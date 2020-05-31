#include "usb.h"

/* Buffer to be used for control requests. */
uint8_t usbControlBuf[128];

static enum usbd_request_return_codes usbCdcAcmControlRequestCb(
    usbd_device *usbd_dev,
    struct usb_setup_data *req,
    uint8_t **buf,
    uint16_t *len __attribute__((unused)),
    void (**complete)(usbd_device *usbd_dev, struct usb_setup_data *req))
{
    (void)complete;
    (void)buf;
    (void)usbd_dev;

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
    (void)ep;
    (void)usbd_dev;

    char buf[64];
    int len = usbd_ep_read_packet(usbd_dev, 0x01, buf, 64);

    if (len) {
        usbd_ep_write_packet(usbd_dev, 0x82, buf, len);
        buf[len] = 0;
    }
}

static void usbCdcAcmSetConfigCb(usbd_device *usbd_dev, uint16_t wValue)
{
    (void)wValue;
    (void)usbd_dev;

    usbd_ep_setup(usbd_dev, 0x01, USB_ENDPOINT_ATTR_BULK, 64, usbCdcAcmReceiveCb);
    usbd_ep_setup(usbd_dev, 0x82, USB_ENDPOINT_ATTR_BULK, 64, NULL);
    usbd_ep_setup(usbd_dev, 0x83, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

    usbd_register_control_callback(
        usbd_dev,
        USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
        USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
        usbCdcAcmControlRequestCb);
}

static void usbTask(void *arg)
{
    usbd_device *usbDev = (usbd_device *)arg;

    for (;;) {
        usbd_poll(usbDev);
        taskYIELD();
    }
}

void usbInit(void)
{
    usbd_device *usbDev;

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

    // TODO set up tx/rx queues
}
