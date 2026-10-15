#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tlv493d.h"
#include "space_mouse.h"

#include "space_mouse_tinyusb/usb_descriptors.h"
#include "space_mouse_tinyusb/usb_hid.h"

#include "configuration.h"

#include "esp_log.h"
#include "esp_task_wdt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tinyusb.h"
#include "class/hid/hid.h"

tlv493d_t tlv493d = {0};

void app_main(void) 
{
    gpio_config_t gpio_configuration = {
        .pin_bit_mask = (1ULL << GPIO_MOUSE_SWITCH_LEFT) | (1ULL << GPIO_MOUSE_SWITCH_RIGHT) | 
                        (1ULL << GPIO_KEY_SWITCH_0) | (1ULL << GPIO_KEY_SWITCH_1) | (1ULL << GPIO_KEY_SWITCH_2),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config(&gpio_configuration);
    
    usb_hid_init();
    
    //space_mouse_initialize_magnetometer(&tlv493d);

    while(true) 
    {
        if(tud_hid_ready())
        {
            static uint8_t usb_hid_report_type = 0;

            switch(usb_hid_report_type)
            {
                case 0: 
                {
                    uint8_t mb = (!gpio_get_level(GPIO_MOUSE_SWITCH_RIGHT) << 1) | !gpio_get_level(GPIO_MOUSE_SWITCH_LEFT);
                    usb_hid_send_mouse(mb, 0, 0, 0);
                    break;
                }
                case 1: 
                {
                    uint8_t keycode = 0;

                    if(!gpio_get_level(GPIO_KEY_SWITCH_0))
                    {
                        keycode = HID_KEY_C;
                    }

                    usb_hid_send_keyboard(keycode, 0);
                    break;
                }
                case 2: 
                {
                    break;
                }
            }

            usb_hid_report_type = (usb_hid_report_type + 1) % 3;

        }

        //space_mouse_get_decoupled_movement(&tlv493d, TLV493D_AXIS_DEADZONE);
        //space_mouse_print_direction(&tlv493d);
        //space_mouse_print_axis(&tlv493d);
        //space_mouse_print_deltas(&tlv493d);
      
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }   
}