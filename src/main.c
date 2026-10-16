#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_task_wdt.h"

#include "tinyusb.h"
#include "class/hid/hid.h"

#include "configuration.h"
#include "tlv493d.h"
#include "space_mouse.h"
#include "space_mouse_input/rotary_encoder.h"
#include "space_mouse_input/key_switch.h"
#include "space_mouse_input/mouse_switch.h"
#include "space_mouse_tinyusb/usb_descriptors.h"
#include "space_mouse_tinyusb/usb_hid.h"

tlv493d_t tlv493d = {0};

rotary_encoder_t rotary_encoder = {0};
key_switch_t key_switches = {0};
mouse_switch_t mouse_switches = {0};

void app_main(void) 
{
    rotary_encoder_init_gpio(&rotary_encoder, GPIO_ROTARY_SWITCH, GPIO_ROTARY_ENCA, GPIO_ROTARY_ENCB);
    key_switch_init_gpio(&key_switches, GPIO_KEY_SWITCH_0, GPIO_KEY_SWITCH_1, GPIO_KEY_SWITCH_2);
    mouse_switch_init_gpio(&mouse_switches, GPIO_MOUSE_SWITCH_LEFT, GPIO_MOUSE_SWITCH_RIGHT);
    
    usb_hid_init();
    
    space_mouse_initialize_magnetometer(&tlv493d);

    while(true) 
    {
        space_mouse_get_decoupled_movement(&tlv493d, TLV493D_AXIS_DEADZONE);        
        space_mouse_handle_usb_hid(&tlv493d, &mouse_switches, &key_switches, &rotary_encoder);
      
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }   
}