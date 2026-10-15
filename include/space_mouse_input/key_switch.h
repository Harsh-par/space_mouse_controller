#ifndef KEY_SWITCH_H
#define KEY_SWITCH_H

#include "space_mouse_tinyusb/usb_hid.h"
#include "space_mouse_tinyusb/usb_descriptors.h"

#include "driver/gpio.h"
#include "esp_timer.h"

#include "tinyusb.h"
#include "class/hid/hid.h"

#define KEY_SWITCH_DEBOUNCE_TIME_US 40000

typedef void (*key_handler_t)(void);

typedef enum key_switch_enum {
    KEY_SWITCH_0,
    KEY_SWITCH_1,
    KEY_SWITCH_2,
    KEY_SWITCH_AMOUNT
} key_switch_enum_t;

typedef struct key_switch {
    uint8_t key_gpio[KEY_SWITCH_AMOUNT];

    uint8_t key_state_current[KEY_SWITCH_AMOUNT];
    uint8_t key_state_previous[KEY_SWITCH_AMOUNT];

    key_handler_t key_handlers[KEY_SWITCH_AMOUNT];
} key_switch_t;

void key_switch_init_gpio(key_switch_t *key_switches, uint8_t gpio_switch_0, uint8_t gpio_switch_1, uint8_t gpio_switch_2);
void key_switch_read(key_switch_t *key_switches);
void key_switch_handle(key_switch_t *key_switches, uint32_t debounce_time_us);

#endif