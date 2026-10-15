#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include "tinyusb.h"

#define TUSB_DESCRIPTOR_STRING_COUNT 3

typedef enum usb_hid_instance {
    HID_INSTANCE_SPACEMOUSE     = 0,
    HID_INSTANCE_MOUSE_KEYBOARD = 1
} usb_hid_instance_t;

typedef enum space_mouse_report_id {
    REPORT_ID_TRANSLATION = 1,
    REPORT_ID_ROTATION    = 2
} space_mouse_report_id_t;

typedef enum mouse_keyboard_report_id {
    REPORT_ID_MOUSE    = 1,
    REPORT_ID_KEYBOARD = 2
} mouse_keyboard_report_id_t;

extern const tusb_desc_device_t tusb_descriptor_device;
extern const char* tusb_descriptor_string[];
extern const uint8_t tusb_descriptor_configuration[];

#endif