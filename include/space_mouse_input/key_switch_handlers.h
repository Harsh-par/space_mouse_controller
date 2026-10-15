#ifndef KEY_SWITCH_HANDLERS_H
#define KEY_SWITCH_HANDLERS_H

#include "space_mouse_tinyusb/usb_hid.h"

#include "tinyusb.h"
#include "class/hid/hid.h"

extern void key_handler_0()
{
    usb_hid_send_keyboard(HID_KEY_A, 0);
}

extern void key_handler_1()
{
    usb_hid_send_keyboard(HID_KEY_B, 0);
}

extern void key_handler_2()
{
    usb_hid_send_keyboard(HID_KEY_C, 0);
}

#endif