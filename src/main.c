#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tlv493d.h"
#include "space_mouse.h"

#include "space_mouse_input/rotary_encoder.h"
#include "space_mouse_input/key_switch.h"
#include "space_mouse_input/mouse_switch.h"

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
rotary_encoder_t rotary_encoder = {0};
key_switch_t key_switches = {0};

void app_main(void) 
{
    rotary_encoder_init_gpio(&rotary_encoder, GPIO_ROTARY_SWITCH, GPIO_ROTARY_ENCA, GPIO_ROTARY_ENCB);
    key_switch_init_gpio(&key_switches, GPIO_KEY_SWITCH_0, GPIO_KEY_SWITCH_1, GPIO_KEY_SWITCH_2);
    mouse_switch_init_gpio(GPIO_MOUSE_SWITCH_LEFT, GPIO_MOUSE_SWITCH_RIGHT);
    
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

                    usb_hid_send_mouse(mb, 0, 0, rotary_encoder.rotary_encoder_layer);
                    break;
                }
                case 1: 
                {
                    key_switch_read(&key_switches);
                    key_switch_handle(&key_switches, KEY_SWITCH_DEBOUNCE_TIME_US);
                    break;
                }
                case 2: 
                {
                    static space_mouse_state_t space_mouse_state = SPACE_MOUSE_STATE_TRANSLATION;
                    
                    space_mouse_data_t space_mouse_data = {0};

                    if(!gpio_get_level(rotary_encoder.gpio_switch))
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