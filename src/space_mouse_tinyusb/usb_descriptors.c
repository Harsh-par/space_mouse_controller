
#include "space_mouse_tinyusb/usb_descriptors.h"
#include "class/hid/hid_device.h"

static const uint8_t hid_spacemouse_report_descriptor[] = {
    0x05, 0x01,             // Usage Page (Generic Desktop)
    0x09, 0x08,             // Usage (Multi-axis Controller)
    0xA1, 0x01,             // Collection (Application)

    // Translation report
    0xA1, 0x00,             // Collection (Physical)
    0x85, 0x01,             // Report ID (Translation)
    0x16, 0xA2, 0xFE,       // Logical Minimum (-350)
    0x26, 0x5E, 0x01,       // Logical Maximum (350)
    0x09, 0x30,             // Usage (X)
    0x09, 0x31,             // Usage (Y)
    0x09, 0x32,             // Usage (Z)
    0x75, 0x10,             // Report Size (16)
    0x95, 0x03,             // Report Count (3)
    0x81, 0x06,             // Input (Data, Variable, Relative)
    0xC0,                   // End Collection

    // Rotation report
    0xA1, 0x00,             // Collection (Physical)
    0x85, 0x02,             // Report ID (Rotation)
    0x09, 0x33,             // Usage (Rx)
    0x09, 0x34,             // Usage (Ry)
    0x09, 0x35,             // Usage (Rz)
    0x75, 0x10,             // Report Size (16)
    0x95, 0x03,             // Report Count (3)
    0x81, 0x06,             // Input (Data, Variable, Relative)
    0xC0,                   // End Collection

    0xC0                    // End Collection
};

static const uint8_t hid_mouse_keyboard_report_descriptor[] = {
    TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)),
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(REPORT_ID_KEYBOARD))
};

const tusb_desc_device_t tusb_descriptor_device = {
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,

    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = 0x046D,
    .idProduct          = 0xC62B,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x00,

    .bNumConfigurations = 0x01
};

const char* tusb_descriptor_string[] = {
    (const char[]){0x09, 0x04},
    "3Dconnexion",
    "SpaceMouse Pro"
};

const uint8_t tusb_descriptor_configuration[] = {
    TUD_CONFIG_DESCRIPTOR(1, 2, 0, TUD_CONFIG_DESC_LEN + (2 * TUD_HID_DESC_LEN), 0x80, 500),
    TUD_HID_DESCRIPTOR(0, 0, HID_ITF_PROTOCOL_NONE, sizeof(hid_spacemouse_report_descriptor), 0x81, 16, 10),
    TUD_HID_DESCRIPTOR(1, 0, HID_ITF_PROTOCOL_NONE, sizeof(hid_mouse_keyboard_report_descriptor), 0x82, 16, 10)
};

const uint8_t* tud_hid_descriptor_report_cb(uint8_t instance)
{
    switch(instance)
    {
        case HID_INSTANCE_SPACEMOUSE:
        {
            return hid_spacemouse_report_descriptor;
        }
        case HID_INSTANCE_MOUSE_KEYBOARD:
        {
            return hid_mouse_keyboard_report_descriptor;
        }
        default: return NULL;
    }
}
