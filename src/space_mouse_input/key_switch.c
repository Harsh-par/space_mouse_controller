#include "space_mouse_input/key_switch.h"
#include "space_mouse_input/key_switch_handlers.h"

void key_switch_init_gpio(key_switch_t *key_switches, uint8_t gpio_switch_0, uint8_t gpio_switch_1, uint8_t gpio_switch_2)
{
    key_switches->key_gpio[KEY_SWITCH_0] = gpio_switch_0;
    key_switches->key_gpio[KEY_SWITCH_1] = gpio_switch_1;
    key_switches->key_gpio[KEY_SWITCH_2] = gpio_switch_2;

    key_switches->key_state_current[KEY_SWITCH_0] = 0;
    key_switches->key_state_current[KEY_SWITCH_1] = 0;
    key_switches->key_state_current[KEY_SWITCH_2] = 0;

    key_switches->key_state_previous[KEY_SWITCH_0] = 0;
    key_switches->key_state_previous[KEY_SWITCH_1] = 0;
    key_switches->key_state_previous[KEY_SWITCH_2] = 0;

    key_switches->key_handlers[KEY_SWITCH_0] = key_handler_0;
    key_switches->key_handlers[KEY_SWITCH_1] = key_handler_1;
    key_switches->key_handlers[KEY_SWITCH_2] = key_handler_2;

    gpio_config_t gpio_configuration_key = {
        .pin_bit_mask = (1ULL << gpio_switch_0) | (1ULL << gpio_switch_1) | (1ULL << gpio_switch_2),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_configuration_key);
}

void key_switch_read(key_switch_t *key_switches)
{
    for(key_switch_enum_t current_switch = KEY_SWITCH_0; current_switch < KEY_SWITCH_AMOUNT; current_switch++)
    {
        key_switches->key_state_previous[current_switch] = key_switches->key_state_current[current_switch];

        key_switches->key_state_current[current_switch] = !gpio_get_level(key_switches->key_gpio[current_switch]);
    }
}

void key_switch_handle(key_switch_t *key_switches, uint32_t debounce_time_us)
{
    static bool key_switch_released = false;

    static uint32_t previous_time_us[KEY_SWITCH_AMOUNT] = {0};
    
    uint32_t current_time_us = esp_timer_get_time();

    if(key_switch_released)
    {
        if(tud_hid_n_ready(HID_INSTANCE_MOUSE_KEYBOARD))
        {
            usb_hid_send_keyboard(HID_KEY_NONE, 0);
            key_switch_released = false;
        }
        return;
    }

    for(key_switch_enum_t current_switch = KEY_SWITCH_0; current_switch < KEY_SWITCH_AMOUNT; current_switch++)
    {
        if(key_switches->key_state_current[current_switch] && !key_switches->key_state_previous[current_switch])
        {
            if(current_time_us - previous_time_us[current_switch] > debounce_time_us)
            {
                key_handler_t key_handler = key_switches->key_handlers[current_switch];

                if(key_handler) 
                {
                    key_handler();
                    key_switch_released = true;
                }

                previous_time_us[current_switch] = current_time_us;
            }
        }
    } 
}