#ifndef ROTARY_ENCODER_H
#define ROTARY_ENCODER_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "driver/gpio.h"

typedef struct rotary_encoder {
    uint8_t gpio_switch;
    uint8_t gpio_phase_a;
    uint8_t gpio_phase_b;

    int16_t rotary_encoder_layer;
    uint8_t rotary_encoder_state;
} rotary_encoder_t;

void rotary_encoder_init_gpio(rotary_encoder_t *rotary_encoder, uint8_t gpio_switch, uint8_t gpio_phase_a, uint8_t gpio_phase_b);

#endif