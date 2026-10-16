#include "space_mouse_input/rotary_encoder.h"

#include "esp_timer.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#define ROTARY_ENCODER_LAYER_COUNT 8
#define ROTARY_ENCODER_FAST_US     40000
#define ROTARY_ENCODER_MEDIUM_US   100000
#define ROTARY_ENCODER_FAST_STEP   4
#define ROTARY_ENCODER_MEDIUM_STEP 2

/*
Encoder rotation handler function based on https://github.com/mo-thunderz/RotaryEncoder
*/
static void rotary_encoder_callback(void *arg)
{
    rotary_encoder_t *rotary_encoder = (rotary_encoder_t*)arg;

    static int8_t  encoder_value = 0;
    static int64_t previous_step_us = 0;
    static int8_t  previous_step_direction = 0;

    static const int8_t encoder_state_table[] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};

    uint8_t current_state = 0;

    if(gpio_get_level(rotary_encoder->gpio_phase_a)) current_state |= 0b10;
    if(gpio_get_level(rotary_encoder->gpio_phase_b)) current_state |= 0b01;

    uint8_t state_index = (rotary_encoder->rotary_encoder_state << 2) | current_state;

    rotary_encoder->rotary_encoder_state = current_state;
    
    encoder_value += encoder_state_table[state_index];

    if(encoder_value >= 4 || encoder_value <= -4)
    {
        int8_t direction = encoder_value > 0 ? 1 : -1;
        int8_t step_size = 1;

        encoder_value = 0;

        int64_t current_time_us = esp_timer_get_time();

        if(previous_step_us != 0 && previous_step_direction == direction)
        {
            int64_t elapsed_us = current_time_us - previous_step_us;

            if(elapsed_us < ROTARY_ENCODER_FAST_US)
            {
                step_size = ROTARY_ENCODER_FAST_STEP;
            }
            else if(elapsed_us < ROTARY_ENCODER_MEDIUM_US)
            {
                step_size = ROTARY_ENCODER_MEDIUM_STEP;
            }
        }

        previous_step_us = current_time_us;
        previous_step_direction = direction;
        
        int32_t scroll = rotary_encoder->rotary_encoder_layer + (direction * step_size);

        if(scroll > ROTARY_ENCODER_LAYER_COUNT) scroll = ROTARY_ENCODER_LAYER_COUNT;
        if(scroll < -ROTARY_ENCODER_LAYER_COUNT) scroll = -ROTARY_ENCODER_LAYER_COUNT;

        rotary_encoder->rotary_encoder_layer = (int16_t)scroll;
    }
}

void rotary_encoder_init_gpio(rotary_encoder_t *rotary_encoder, uint8_t gpio_switch, uint8_t gpio_phase_a, uint8_t gpio_phase_b)
{
    rotary_encoder->gpio_switch = gpio_switch;
    rotary_encoder->gpio_phase_a = gpio_phase_a;
    rotary_encoder->gpio_phase_b = gpio_phase_b;

    rotary_encoder->rotary_encoder_state = 0;
    rotary_encoder->rotary_encoder_layer = 0;
    
    gpio_config_t gpio_configuration_switch = {
        .pin_bit_mask = (1ULL << gpio_switch),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };
  
    gpio_config(&gpio_configuration_switch);

    gpio_config_t gpio_configuration_phase = {
        .pin_bit_mask = (1ULL << gpio_phase_a) | (1ULL << gpio_phase_b),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_ANYEDGE
    };

    gpio_config(&gpio_configuration_phase);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(gpio_phase_a, rotary_encoder_callback, rotary_encoder);
    gpio_isr_handler_add(gpio_phase_b, rotary_encoder_callback, rotary_encoder);
}