#ifndef CASECOMMANDER_USB_H
#define CASECOMMANDER_USB_H

#include <stdint.h>

#include <libopencm3/usb/cdc.h>
#include <libopencm3/usb/usbd.h>

#include <FreeRTOS.h>
#include <task.h>

/**
 * USB Device descriptor
 */
static const struct usb_device_descriptor usbDevDesc = {
    .bLength            = USB_DT_DEVICE_SIZE,
    .bDescriptorType    = USB_DT_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = USB_CLASS_CDC,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,
    .bMaxPacketSize0    = 64,
    .idVendor           = 0x0483,
    .idProduct          = 0x5740,
    .bcdDevice          = 0x0001,
    .iManufacturer      = 1,
    .iProduct           = 2,
    .iSerialNumber      = 3,
    .bNumConfigurations = 1,
};

/**
 * USB CDC ACM communication endpoint descriptors (1x interrupt IN)
 */
static const struct usb_endpoint_descriptor usbCommEpDesc[] = {{
    .bLength          = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType  = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x83,
    .bmAttributes     = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize   = 16,
    .bInterval        = 255,
}};

/**
 * USB CDC ACM data endpoint descriptors (1x bulk OUT, 1x bulk IN)
 */
static const struct usb_endpoint_descriptor usbDataEpDesc[] = {
    {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = 0x01,
        .bmAttributes     = USB_ENDPOINT_ATTR_BULK,
        .wMaxPacketSize   = 64,
        .bInterval        = 1,
    },
    {
        .bLength          = USB_DT_ENDPOINT_SIZE,
        .bDescriptorType  = USB_DT_ENDPOINT,
        .bEndpointAddress = 0x82,
        .bmAttributes     = USB_ENDPOINT_ATTR_BULK,
        .wMaxPacketSize   = 64,
        .bInterval        = 1,
    }};

/**
 * USB CDC ACM functional descriptors
 */
static const struct {
    struct usb_cdc_header_descriptor usbCdcHeaderDesc;
    struct usb_cdc_call_management_descriptor usbCdcCallMgmtDesc;
    struct usb_cdc_acm_descriptor usbCdcAcmDesc;
    struct usb_cdc_union_descriptor usbCdcUnionDesc;
} __attribute__((packed)) usbCdcFunctionalDescs = {
    .usbCdcHeaderDesc =
        {
            .bFunctionLength    = sizeof(struct usb_cdc_header_descriptor),
            .bDescriptorType    = CS_INTERFACE,
            .bDescriptorSubtype = USB_CDC_TYPE_HEADER,
            .bcdCDC             = 0x0110,
        },
    .usbCdcCallMgmtDesc =
        {
            .bFunctionLength    = sizeof(struct usb_cdc_call_management_descriptor),
            .bDescriptorType    = CS_INTERFACE,
            .bDescriptorSubtype = USB_CDC_TYPE_CALL_MANAGEMENT,
            .bmCapabilities     = 0,
            .bDataInterface     = 1,
        },
    .usbCdcAcmDesc =
        {
            .bFunctionLength    = sizeof(struct usb_cdc_acm_descriptor),
            .bDescriptorType    = CS_INTERFACE,
            .bDescriptorSubtype = USB_CDC_TYPE_ACM,
            .bmCapabilities     = 0,
        },
    .usbCdcUnionDesc =
        {
            .bFunctionLength        = sizeof(struct usb_cdc_union_descriptor),
            .bDescriptorType        = CS_INTERFACE,
            .bDescriptorSubtype     = USB_CDC_TYPE_UNION,
            .bControlInterface      = 0,
            .bSubordinateInterface0 = 1,
        },
};

/**
 * USB CDC ACM communication interface descriptors
 */
static const struct usb_interface_descriptor usbCommIfDescs[] = {{
    .bLength            = USB_DT_INTERFACE_SIZE,
    .bDescriptorType    = USB_DT_INTERFACE,
    .bInterfaceNumber   = 0,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 1,
    .bInterfaceClass    = USB_CLASS_CDC,
    .bInterfaceSubClass = USB_CDC_SUBCLASS_ACM,
    .bInterfaceProtocol = USB_CDC_PROTOCOL_AT,
    .iInterface         = 0,

    .endpoint = usbCommEpDesc,
    .extra    = &usbCdcFunctionalDescs,
    .extralen = sizeof(usbCdcFunctionalDescs),
}};

/**
 * USB CDC ACM data interface descriptors
 */
static const struct usb_interface_descriptor usbDataIfDescs[] = {{
    .bLength            = USB_DT_INTERFACE_SIZE,
    .bDescriptorType    = USB_DT_INTERFACE,
    .bInterfaceNumber   = 1,
    .bAlternateSetting  = 0,
    .bNumEndpoints      = 2,
    .bInterfaceClass    = USB_CLASS_DATA,
    .bInterfaceSubClass = 0,
    .bInterfaceProtocol = 0,
    .iInterface         = 0,

    .endpoint = usbDataEpDesc,
}};

/**
 * USB interfaces/alternative settings mapping
 */
static const struct usb_interface usbInterfaces[] = {
    {
        .num_altsetting = 1,
        .altsetting     = usbCommIfDescs,
    },
    {
        .num_altsetting = 1,
        .altsetting     = usbDataIfDescs,
    },
};

/**
 * USB interfaces configuration descriptor
 */
static const struct usb_config_descriptor usbConfigDesc = {
    .bLength             = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType     = USB_DT_CONFIGURATION,
    .wTotalLength        = 0,
    .bNumInterfaces      = 2,
    .bConfigurationValue = 1,
    .iConfiguration      = 0,
    .bmAttributes        = 0x80,
    .bMaxPower           = 0x32,

    .interface = usbInterfaces,
};

static const char *usbStrings[] = {
    "LaXiS.it",
    "CaseCommander",
    "1234",
};

void usbInit(void);

#endif
