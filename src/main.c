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
    gpio_config_t gpio_configuration_switch = {
        .pin_bit_mask = (1ULL << GPIO_MOUSE_SWITCH_LEFT) | (1ULL << GPIO_MOUSE_SWITCH_RIGHT) | (1ULL << GPIO_ROTARY_SWITCH),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config_t gpio_configuration_key = {
        .pin_bit_mask = (1ULL << GPIO_KEY_SWITCH_0) | (1ULL << GPIO_KEY_SWITCH_1) | (1ULL << GPIO_KEY_SWITCH_2),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE
    };

    gpio_config_t gpio_configuration_rotary_encoder = {
        .pin_bit_mask = (1ULL << GPIO_ROTARY_ENCA) | (1ULL << GPIO_ROTARY_ENCB),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,   
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_ANYEDGE
    };

    gpio_config(&gpio_configuration_switch);
    gpio_config(&gpio_configuration_key);
    gpio_config(&gpio_configuration_rotary_encoder);
    
    usb_hid_init();
    
    space_mouse_initialize_magnetometer(&tlv493d);

    while(true) 
    {
        space_mouse_get_decoupled_movement(&tlv493d, TLV493D_AXIS_DEADZONE);
        //space_mouse_print_direction(&tlv493d);
        //space_mouse_print_axis(&tlv493d);
        //space_mouse_print_deltas(&tlv493d);

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
                    static space_mouse_state_t space_mouse_state = SPACE_MOUSE_STATE_TRANSLATION;
                    
                    space_mouse_data_t space_mouse_data = {0};

                    if(!gpio_get_level(GPIO_KEY_SWITCH_1))
                    {
                        space_mouse_state = (space_mouse_state + 1) % SPACE_MOUSE_STATE_TOTAL;
                    }

                    switch(space_mouse_state)
                    {
                        case SPACE_MOUSE_STATE_TRANSLATION:
                        {
                            space_mouse_data.x = tlv493d.axis_x;
                            space_mouse_data.y = tlv493d.axis_y;
                            space_mouse_data.z = tlv493d.axis_z;
                            break;
                        }
                        case SPACE_MOUSE_STATE_ROTATION:
                        {
                            space_mouse_data.rx = tlv493d.axis_x;
                            space_mouse_data.ry = tlv493d.axis_y;
                            space_mouse_data.rz = tlv493d.axis_z;
                            break;
                        }
                        default: break;
                    }

                    usb_hid_send_spacemouse(&space_mouse_data);

                    break;
                }
            }

            usb_hid_report_type = (usb_hid_report_type + 1) % 3;
        }
      
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }   
}