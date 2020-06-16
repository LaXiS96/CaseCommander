#include "usb.h"

MessageBufferHandle_t usbRxMessages = NULL;

static volatile bool isConfigured       = false;
static uint8_t       usbControlBuf[128] = {};

static SemaphoreHandle_t    txSemaphore = NULL;
static StreamBufferHandle_t rxStream    = NULL;
static StreamBufferHandle_t txStream    = NULL;

static enum usbd_request_return_codes usbCdcAcmControlRequestCb(
    usbd_device *          usbDev,
    struct usb_setup_data *req,
    uint8_t **             buf,
    uint16_t *             len,
    void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
    (void)usbDev;
    (void)buf;
    (void)len;
    (void)complete;

    switch (req->bRequest) {
    case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
        return USBD_REQ_HANDLED;
    case USB_CDC_REQ_SET_LINE_CODING:
        return USBD_REQ_HANDLED;
    }

    return USBD_REQ_NOTSUPP;
}

static void usbCdcAcmReceiveCb(usbd_device *usbDev, uint8_t ep)
{
    size_t rxAvail = xStreamBufferSpacesAvailable(rxStream);
    char   buf[64];

    if (rxAvail == 0)
        return;

    size_t len = sizeof(buf) < rxAvail ? sizeof(buf) : rxAvail;

    len = usbd_ep_read_packet(usbDev, ep, buf, len);

    xStreamBufferSend(rxStream, buf, len, 0);
}

static void usbCdcAcmTransmitCb(usbd_device *usbDev, uint8_t ep)
{
    (void)usbDev;
    (void)ep;

    xSemaphoreGive(txSemaphore);
}

static void usbCdcAcmSetConfigCb(usbd_device *usbDev, uint16_t wValue)
{
    (void)wValue;

    usbd_ep_setup(usbDev, CC_USB_EP_DATA_OUT, USB_ENDPOINT_ATTR_BULK, 64, usbCdcAcmReceiveCb);
    usbd_ep_setup(usbDev, CC_USB_EP_DATA_IN, USB_ENDPOINT_ATTR_BULK, 64, usbCdcAcmTransmitCb);
    usbd_ep_setup(usbDev, CC_USB_EP_COMM_IN, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

    usbd_register_control_callback(
        usbDev,
        USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        usbCdcAcmControlRequestCb);

    isConfigured = true;
}

/**
 * asdasdasd
 */
static void usbTask(void *arg)
{
    usbd_device * usbDev = (usbd_device *)arg;
    static char   buf[CC_USB_MAX_PACKET_SIZE];
    static char * message    = NULL;
    static size_t messageLen = 0;

    // Reassemble received messages and push them to the message buffer

    for (;;) {
        usbd_poll(usbDev);

        size_t rxLen = xStreamBufferReceive(rxStream, buf, sizeof(buf), 0);
        if (rxLen > 0) {
            // Find LF character in received data
            char * pos     = memchr(buf, '\n', rxLen);
            bool   lfFound = (pos != NULL);
            size_t copyLen = lfFound ? (size_t)(pos - buf) : rxLen;

            // Append data to current message
            if (copyLen > 0) {
                if (message != NULL) {
                    message = realloc(message, messageLen + copyLen);
                    memcpy(message + messageLen, buf, copyLen);
                } else {
                    message = malloc(copyLen);
                    memcpy(message, buf, copyLen);
                }
                messageLen += copyLen;
            }

            // Send message if LF was found
            if (lfFound && message != NULL && messageLen > 0) {
                xMessageBufferSend(usbRxMessages, message, messageLen, 0);

                free(message);
                message    = NULL;
                messageLen = 0;
            }
        } else {
            taskYIELD();
        }
    }
}

static void usbTxTask(void *arg)
{
    usbd_device *usbDev = (usbd_device *)arg;
    static char  buf[CC_USB_MAX_PACKET_SIZE];
    static bool  zlpNeeded = false;

    // Send as much data as available from the buffer and terminate with a Zero Length Packet if we
    // just wrote a full packet and no new data is waiting in the buffer

    for (;;) {
        if (isConfigured) {
            size_t txLen = xStreamBufferReceive(txStream, buf, sizeof(buf), 0);

            if (txLen > 0) {
                xSemaphoreTake(txSemaphore, portMAX_DELAY);
                txLen     = usbd_ep_write_packet(usbDev, CC_USB_EP_DATA_IN, buf, txLen);
                zlpNeeded = txLen == CC_USB_MAX_PACKET_SIZE;
            } else {
                if (zlpNeeded) {
                    xSemaphoreTake(txSemaphore, portMAX_DELAY);
                    usbd_ep_write_packet(usbDev, CC_USB_EP_DATA_IN, NULL, 0);
                    zlpNeeded = false;
                }
                taskYIELD();
            }
        } else {
            taskYIELD();
        }
    }
}

void usbReenumerate(void)
{
    // https://github.com/jeanthom/DirtyJTAG/issues/10
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
    gpio_clear(GPIOA, GPIO12);

    // ~195ms delay (as verified with oscilloscope)
    // TODO maybe setup SysTick and use that for deterministic delay
    for (int i = 0; i < 2000000; i++)
        __asm__("nop");

    rcc_periph_clock_disable(RCC_GPIOA);
}

void usbInit(void)
{
    usbd_device *usbDev;

    rxStream      = xStreamBufferCreate(CC_STREAM_BUFFER_SIZE, 1);
    txStream      = xStreamBufferCreate(CC_STREAM_BUFFER_SIZE, 1);
    usbRxMessages = xMessageBufferCreate(CC_MESSAGE_BUFFER_SIZE);
    txSemaphore   = xSemaphoreCreateBinary();

    configASSERT(rxStream != NULL);
    configASSERT(txStream != NULL);
    configASSERT(usbRxMessages != NULL);
    configASSERT(txSemaphore != NULL);

    xSemaphoreGive(txSemaphore);

    usbDev = usbd_init(
        &st_usbfs_v1_usb_driver,
        &usbDevDesc,
        &usbConfigDesc,
        usbStrings,
        3,
        usbControlBuf,
        sizeof(usbControlBuf));

    usbd_register_set_config_callback(usbDev, usbCdcAcmSetConfigCb);

    xTaskCreate(usbTask, "usb", configMINIMAL_STACK_SIZE, usbDev, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(
        usbTxTask, "usbTx", configMINIMAL_STACK_SIZE, usbDev, configMAX_PRIORITIES - 1, NULL);
}

size_t usbWrite(const char *data, size_t length)
{
    return xStreamBufferSend(txStream, data, length, 0);
}

size_t usbWriteString(const char *str)
{
    return usbWrite(str, strlen(str));
}
