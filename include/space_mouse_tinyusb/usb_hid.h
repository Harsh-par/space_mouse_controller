#ifndef USB_HID_H
#define USB_HID_H

#include <stdint.h>
#include <stdbool.h>

typedef struct space_mouse_data {
    int16_t x;
    int16_t y;
    int16_t z;
    int16_t rx;
    int16_t ry;
    int16_t rz;
} space_mouse_data_t;

void usb_hid_init(void);

void usb_hid_send_mouse(uint8_t buttons, int8_t x, int8_t y, int8_t scroll);
void usb_hid_send_keyboard(uint8_t keycode, uint8_t modifier);
void usb_hid_send_spacemouse(int16_t x, int16_t y, int16_t z, int16_t rx, int16_t ry, int16_t rz);

#endif