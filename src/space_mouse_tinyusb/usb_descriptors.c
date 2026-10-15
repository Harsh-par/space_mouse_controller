#include "space_mouse_tinyusb/usb_descriptors.h"
#include "class/hid/hid_device.h" 

#define TUD_HID_REPORT_DESC_SPACEMOUSE(report_id) \
    0x05, 0x01,        /* Usage Page (Generic Desktop) */ \
    0x09, 0x08,        /* Usage (Multi-axis Controller) */ \
    0xA1, 0x01,        /* Collection (Application) */ \
    0x85, report_id,   /* Report ID */ \
    0x05, 0x01,        /*   Usage Page (Generic Desktop) */ \
    0x09, 0x30,        /*   Usage (X) */ \
    0x09, 0x31,        /*   Usage (Y) */ \
    0x09, 0x32,        /*   Usage (Z) */ \
    0x09, 0x33,        /*   Usage (Rx) */ \
    0x09, 0x34,        /*   Usage (Ry) */ \
    0x09, 0x35,        /*   Usage (Rz) */ \
    0x16, 0x0C, 0xFE,  /*   Logical Minimum (-500) */ \
    0x26, 0xF4, 0x01,  /*   Logical Maximum (500) */ \
    0x75, 0x10,        /*   Report Size (16 bits) */ \
    0x95, 0x06,        /*   Report Count (6 axes) */ \
    0x81, 0x02,        /*   Input (Data, Variable, Absolute) */ \
    0xC0               /* End Collection */

const uint8_t hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD)),
    TUD_HID_REPORT_DESC_SPACEMOUSE(REPORT_ID_SPACEMOUSE)
};

const tusb_desc_device_t tusb_descriptor_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = 0xCAFE, // 3Dconnexion VID
    .idProduct          = 0xC62E, // SpaceMouse Compact PID
    .bcdDevice          = 0x0438,
    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,
    .bNumConfigurations = 0x01
};

const char* tusb_descriptor_string[] = {
    (const char[]){0x09, 0x04},
    "spacemouse",              
    "spacemouse",       
    "123456"                   
};

const uint8_t tusb_descriptor_configuration[] = { 
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUD_CONFIG_DESC_LEN + TUD_HID_DESC_LEN, 0x80, 500), 
    TUD_HID_DESCRIPTOR(0, 0, HID_ITF_PROTOCOL_NONE, sizeof(hid_report_descriptor), 0x81, 16, 10) 
};

const uint8_t* tud_hid_descriptor_report_cb(uint8_t instance) 
{
    return hid_report_descriptor;
}