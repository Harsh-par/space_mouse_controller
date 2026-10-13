#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tlv493d.h"
#include "space_mouse.h"
#include "configuration.h"

#include "esp_log.h"
#include "esp_task_wdt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tinyusb.h"

tlv493d_t tlv493d = {0};

void app_main(void) 
{
    gpio_config_t gpio_configuration = {
        .pin_bit_mask = (1ULL << GPIO_MOUSE_SWITCH_LEFT) | (1ULL << GPIO_MOUSE_SWITCH_RIGHT),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_configuration);

    space_mouse_initialize_magnetometer(&tlv493d);

    while(true) 
    {
        space_mouse_get_decoupled_movement(&tlv493d, TLV493D_AXIS_DEADZONE);
        space_mouse_print_direction(&tlv493d);
        space_mouse_print_axis(&tlv493d);
        space_mouse_print_deltas(&tlv493d);
      
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }   
}