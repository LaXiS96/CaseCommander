#include "usb.h"

// TODO rename to "comm" and handle all PC communication in here

MessageBufferHandle_t usbRxMessages;

static bool    isConfigured;
static uint8_t usbControlBuf[128];

static StreamBufferHandle_t rxStream;
static StreamBufferHandle_t txStream;

static enum usbd_request_return_codes usbCdcAcmControlRequestCb(
    usbd_device *          usbd_dev,
    struct usb_setup_data *req,
    uint8_t **             buf,
    uint16_t *             len,
    void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
    (void)usbd_dev;
    (void)buf;
    (void)len;
    (void)complete;

    switch (req->bRequest) {
    case USB_CDC_REQ_SET_CONTROL_LINE_STATE: {
        return USBD_REQ_HANDLED;
    }
    case USB_CDC_REQ_SET_LINE_CODING:
        return USBD_REQ_HANDLED;
    }

    return USBD_REQ_NOTSUPP;
}

static void usbCdcAcmReceiveCb(usbd_device *usbd_dev, uint8_t ep)
{
    size_t rxAvail = xStreamBufferSpacesAvailable(rxStream);
    char   buf[64];

    if (rxAvail == 0)
        return;

    size_t len = sizeof(buf) < rxAvail ? sizeof(buf) : rxAvail;

    len = usbd_ep_read_packet(usbd_dev, ep, buf, len);

    xStreamBufferSend(rxStream, buf, len, 0);
}

static void usbCdcAcmSetConfigCb(usbd_device *usbd_dev, uint16_t wValue)
{
    (void)wValue;

    usbd_ep_setup(usbd_dev, CC_USB_EP_DATA_OUT, USB_ENDPOINT_ATTR_BULK, 64, usbCdcAcmReceiveCb);
    usbd_ep_setup(usbd_dev, CC_USB_EP_DATA_IN, USB_ENDPOINT_ATTR_BULK, 64, NULL);
    usbd_ep_setup(usbd_dev, CC_USB_EP_COMM_IN, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

    usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        usbCdcAcmControlRequestCb);

    isConfigured = true;
}

static void usbTask(void *arg)
{
    usbd_device *usbDev = (usbd_device *)arg;
    static char  buf[CC_USB_MAX_PACKET_SIZE - 1]; // -1 because sending full packets is a PITA
    char *       message    = NULL;
    size_t       messageLen = 0;

    for (;;) {
        usbd_poll(usbDev);

        // Reassemble received messages from RX stream and push to message buffer

        size_t rxLen = xStreamBufferReceive(rxStream, buf, sizeof(buf), 0);
        if (rxLen > 0) {
            usbWriteString(cc_sprintf("SRX%d\n", rxLen));
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
                usbWriteString(cc_sprintf("SEND%d\n", messageLen));

                xMessageBufferSend(usbRxMessages, message, messageLen, 0);

                free(message);
                message    = NULL;
                messageLen = 0;
            }
        }

        if (isConfigured) {
            // TODO MUST SYNCHRONIZE USB WRITES, need to wait for last packet to be sent before
            // writing a new one (use in endpoint callback to give semaphore to taking task)

            // static bool zlpNeeded = false;
            // Send data from TX stream
            size_t txLen = xStreamBufferReceive(txStream, buf, sizeof(buf), 0);
            if (txLen > 0) {
                // ATTENTION: do not exceed the endpoint's max packet size when sending, the driver
                // does not split packets automagically so we must do it ourselves
                usbd_ep_write_packet(usbDev, CC_USB_EP_DATA_IN, buf, txLen);
                // zlpNeeded = txLen % CC_USB_MAX_PACKET_SIZE == 0;

                // TODO don't send zlp if not needed (eg. when message is longer than 64)
                // Delay a bit and send a zero length packet:
                // - delay is needed because... it doesn't work without it
                // - zero length packet is needed to close a full packet (64bytes = max packet size)
                //
                // usbd_ep_write_packet(usbDev, CC_USB_EP_DATA_IN, "", 1);
                // } else if (zlpNeeded) {
                //     usbd_ep_write_packet(usbDev, CC_USB_EP_DATA_IN, NULL, 0);
            }
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

    usbDev = usbd_init(
        &st_usbfs_v1_usb_driver,
        &usbDevDesc,
        &usbConfigDesc,
        usbStrings,
        3,
        usbControlBuf,
        sizeof(usbControlBuf));

    usbd_register_set_config_callback(usbDev, usbCdcAcmSetConfigCb);

    xTaskCreate(usbTask, "USB", 200, usbDev, configMAX_PRIORITIES - 1, NULL);
}

size_t usbWrite(const char *data, size_t length)
{
    // TODO use FromISR if calling from isr
    return xStreamBufferSend(txStream, data, length, 0);
}

size_t usbWriteString(const char *str)
{
    return usbWrite(str, strlen(str));
}
