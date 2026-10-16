#ifndef MOUSE_SWITCH_H
#define MOUSE_SWITCH_H

#include "space_mouse_tinyusb/usb_hid.h"

#include "esp_timer.h"
#include "driver/gpio.h"

#define MOUSE_SWITCH_DEBOUNCE_TIME_US 15000

typedef enum mouse_switch_enum {
    MOUSE_SWITCH_LEFT,
    MOUSE_SWITCH_RIGHT,
    MOUSE_SWITCH_AMOUNT
} mouse_switch_enum_t;

typedef struct mouse_switch {
    uint8_t mouse_gpio[MOUSE_SWITCH_AMOUNT];

    uint8_t mouse_state_current[MOUSE_SWITCH_AMOUNT];
    uint8_t mouse_state_previous[MOUSE_SWITCH_AMOUNT];
} mouse_switch_t;

void mouse_switch_init_gpio(mouse_switch_t *mouse_switch, uint8_t gpio_switch_left, uint8_t gpio_switch_right);
void mouse_switch_read(mouse_switch_t *mouse_switch);
void mouse_switch_handle(mouse_switch_t *mouse_switch, int16_t mouse_scroll, uint32_t debounce_time_us);

#endif