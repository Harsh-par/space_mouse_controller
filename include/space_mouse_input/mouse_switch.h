#ifndef MOUSE_SWITCH_H
#define MOUSE_SWITCH_H

#include "driver/gpio.h"

void mouse_switch_init_gpio(uint8_t gpio_switch_left, uint8_t gpio_switch_right);

#endif