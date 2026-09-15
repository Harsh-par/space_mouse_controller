#ifndef TLV493D_H
#define TLV493D_H

#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"

#include "tlv493d_configuration.h"
#include "configuration.h"

typedef struct {
    uint8_t i2c_gpio_sda;
    uint8_t i2c_gpio_scl;
    uint8_t i2c_port;
    uint8_t i2c_address;
    int16_t m_x;
    int16_t m_y;
    int16_t m_z;
    int16_t m_base_x;
    int16_t m_base_y;
    int16_t m_base_z;
    int16_t m_delta_x;
    int16_t m_delta_y;
    int16_t m_delta_z;
    float axis_x;
    float axis_y;
    float axis_z;
} tlv493d_t;

void tlv493d_i2c_init(tlv493d_t *tlv493d, uint8_t i2c_gpio_sda, uint8_t i2c_gpio_scl, uint8_t i2c_port, uint8_t i2c_address, bool configure_i2c);
void tlv493d_device_init(tlv493d_t *tlv493d);
void tlv493d_read(tlv493d_t *tlv493d);
void tlv493d_calibrate_center(tlv493d_t *tlv493d, uint32_t calibration_samples);
void tlv493d_get_decoupled_movement(tlv493d_t *tlv493d, float deadzone);

#endif