#include "space_mouse_input/mouse_switch.h"

void mouse_switch_init_gpio(uint8_t gpio_switch_left, uint8_t gpio_switch_right)
{
    gpio_config_t gpio_configuration_mouse = {
        .pin_bit_mask = (1ULL << gpio_switch_left) | (1ULL << gpio_switch_right),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_configuration_mouse);
}