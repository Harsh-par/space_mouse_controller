#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tlv493d.h"

#include "configuration.h"

#include "esp_log.h"
#include "esp_task_wdt.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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

    tlv493d_i2c_init(&tlv493d, I2C_MASTER_SDA_GPIO, I2C_MASTER_SCL_GPIO, I2C_MASTER_PORT, I2C_ADDRESS_TLV493D, true);
    tlv493d_device_init(&tlv493d);
    tlv493d_calibrate_center(&tlv493d, TLV493D_CALIBRATION_SAMPLES);

    static bool calibration_mode = false;

    while(true) 
    {
        if(!calibration_mode)
        {
            tlv493d_read(&tlv493d);
            tlv493d_get_decoupled_movement(&tlv493d, TLV493D_AXIS_DEADZONE);

            float abs_x = fabsf(tlv493d.axis_x);
            float abs_y = fabsf(tlv493d.axis_y);
            float abs_z = fabsf(tlv493d.axis_z);
            const char *direction = "IDLE";

            if (abs_x > 0.0f || abs_y > 0.0f || abs_z > 0.0f) {
                if (abs_x >= abs_y && abs_x >= abs_z) {
                    direction = (tlv493d.axis_x > 0.0f) ? "RIGHT" : "LEFT";
                } else if (abs_y >= abs_x && abs_y >= abs_z) {
                    direction = (tlv493d.axis_y > 0.0f) ? "FORWARD" : "BACKWARD";
                } else if (abs_z > abs_x && abs_z > abs_y){
                    direction = (tlv493d.axis_z < 0.0f) ? "PUSH DOWN" : "PULL UP";
                }
                
            }     
            //printf("delta %d %d %d\n", tlv493d.m_delta_x, tlv493d.m_delta_y, tlv493d.m_delta_z);
            printf("Direction: %-10s %lf %lf %lf\n", direction, abs_x, abs_y, abs_z);
        }

        if(calibration_mode)
        {
            static bool read = false;
            // printf("normal %d %d %d\n", tlv493d.m_x, tlv493d.m_y, tlv493d.m_z);
            // printf("center %d %d %d\n", tlv493d.m_base_x, tlv493d.m_base_y, tlv493d.m_base_z);
            if(gpio_get_level(GPIO_MOUSE_SWITCH_LEFT) == false)
            {
                printf("startinng reading");
                vTaskDelay(pdMS_TO_TICKS(2000));
                read = true;
            }

            while(read)
            {
                for(int i=0; i<50; i++)
                {
                    tlv493d_read(&tlv493d);
                    tlv493d_get_decoupled_movement(&tlv493d, TLV493D_AXIS_DEADZONE);
                    printf("delta %d %d %d\n", tlv493d.m_delta_x, tlv493d.m_delta_y, tlv493d.m_delta_z);
                }
                printf("reading done\n");
                read = false;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }   
}