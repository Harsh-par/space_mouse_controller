#include "space_mouse_input/mouse_switch.h"

void mouse_switch_init_gpio(mouse_switch_t *mouse_switch, uint8_t gpio_switch_left, uint8_t gpio_switch_right)
{
    mouse_switch->mouse_gpio[MOUSE_SWITCH_LEFT]  = gpio_switch_left;
    mouse_switch->mouse_gpio[MOUSE_SWITCH_RIGHT] = gpio_switch_right;

    mouse_switch->mouse_state_current[MOUSE_SWITCH_LEFT]  = 0;
    mouse_switch->mouse_state_current[MOUSE_SWITCH_RIGHT] = 0;

    mouse_switch->mouse_state_previous[MOUSE_SWITCH_LEFT]  = 0;
    mouse_switch->mouse_state_previous[MOUSE_SWITCH_RIGHT] = 0;

    gpio_config_t gpio_configuration_mouse = {
        .pin_bit_mask = (1ULL << gpio_switch_left) | (1ULL << gpio_switch_right),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_configuration_mouse);
}

void mouse_switch_read(mouse_switch_t *mouse_switch)
{
    for(mouse_switch_enum_t current_switch = MOUSE_SWITCH_LEFT; current_switch < MOUSE_SWITCH_AMOUNT; current_switch++)
    {
        mouse_switch->mouse_state_previous[current_switch] = mouse_switch->mouse_state_current[current_switch];

        mouse_switch->mouse_state_current[current_switch] = !gpio_get_level(mouse_switch->mouse_gpio[current_switch]);
    }
}

void mouse_switch_handle(mouse_switch_t *mouse_switch, int16_t mouse_scroll, uint32_t debounce_time_us)
{
    static uint32_t previous_time_us[MOUSE_SWITCH_AMOUNT] = {0};

    static uint8_t mouse_state_debounced[MOUSE_SWITCH_AMOUNT] = {0};

    uint32_t current_time_us = esp_timer_get_time();
    uint8_t mouse_buttons = 0;

    for(mouse_switch_enum_t current_switch = MOUSE_SWITCH_LEFT; current_switch < MOUSE_SWITCH_AMOUNT; current_switch++)
    {
        if(mouse_switch->mouse_state_current[current_switch] != mouse_switch->mouse_state_previous[current_switch])
        {
            previous_time_us[current_switch] = current_time_us;
        }

        if(current_time_us - previous_time_us[current_switch] >= debounce_time_us)
        {
            mouse_state_debounced[current_switch] = mouse_switch->mouse_state_current[current_switch];
        }

        mouse_buttons |= mouse_state_debounced[current_switch] << current_switch;
    }

    usb_hid_send_mouse(mouse_buttons, 0, 0, mouse_scroll);
}