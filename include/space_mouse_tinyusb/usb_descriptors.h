#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

#include "tinyusb.h"

#define TUSB_DESCRIPTOR_STRING_COUNT 3

typedef enum space_mouse_report_id {
    REPORT_ID_MOUSE      = 1,
    REPORT_ID_KEYBOARD   = 2,
    REPORT_ID_SPACEMOUSE = 3
} space_mouse_report_id_t;

extern const tusb_desc_device_t tusb_descriptor_device;
extern const char* tusb_descriptor_string[];
extern const uint8_t tusb_descriptor_configuration[];

#endif