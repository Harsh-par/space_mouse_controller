#include "tlv493d.h"

static const float space_decoupling_matrix[3][3] = {
    {    0.0859f,    -2.4073f,     0.4259f},
    {    2.7996f,     0.1626f,     0.3729f},
    {   -0.0406f,     0.1858f,     0.9977f}
};

static int16_t tlv493d_unpack_12bit(uint8_t byte_msb, uint8_t byte_lsb) 
{
    uint16_t value = (byte_msb << 4) | (byte_lsb & 0x0F);
    
    if(value >= 2048) return value - 4096;    

    return value;
}

static uint8_t tlv493d_calculate_even_parity(const uint8_t *data, uint8_t data_length) 
{
    uint8_t parity = 0;

    for (uint8_t i = 0; i < data_length; i++) 
    {
        uint8_t temp = data[i];

        while(temp) 
        {
            parity ^= (temp & 1);
            temp >>= 1;
        }
    }

    return parity;
}

static void tlv493d_read_raw(tlv493d_t *tlv493d) 
{
    uint8_t buffer_data[6] = {0};

    esp_err_t error = i2c_master_read_from_device(tlv493d->i2c_port, tlv493d->i2c_address, buffer_data, sizeof(buffer_data), I2C_MASTER_TIMEOUT_MS);
    
    if(error != ESP_OK) 
    {
        printf("I2C READ ERROR: %d\n", error); 
        return;
    }

    tlv493d->m_x = tlv493d_unpack_12bit(buffer_data[0], buffer_data[4] >> 4);
    tlv493d->m_y = tlv493d_unpack_12bit(buffer_data[1], buffer_data[4] & 0x0F);
    tlv493d->m_z = tlv493d_unpack_12bit(buffer_data[2], buffer_data[5] & 0x0F);
}

void tlv493d_i2c_init(tlv493d_t *tlv493d, uint8_t i2c_gpio_sda, uint8_t i2c_gpio_scl, uint8_t i2c_port, uint8_t i2c_address, bool configure_i2c) 
{
    tlv493d->i2c_gpio_sda = i2c_gpio_sda;
    tlv493d->i2c_gpio_scl = i2c_gpio_scl;
    tlv493d->i2c_port     = i2c_port;
    tlv493d->i2c_address  = i2c_address;

    if(configure_i2c)
    {
        i2c_config_t i2c_config = {
            .mode             = I2C_MODE_MASTER,
            .sda_io_num       = tlv493d->i2c_gpio_sda,
            .scl_io_num       = tlv493d->i2c_gpio_scl,
            .sda_pullup_en    = GPIO_PULLUP_ENABLE,
            .scl_pullup_en    = GPIO_PULLUP_ENABLE,
            .master.clk_speed = I2C_MASTER_FREQ_HZ,
        };

        i2c_param_config(tlv493d->i2c_port, &i2c_config);
        i2c_driver_install(tlv493d->i2c_port, i2c_config.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    }
}

void tlv493d_device_init(tlv493d_t *tlv493d) 
{    
    tlv493d->m_x = 0;
    tlv493d->m_y = 0;
    tlv493d->m_z = 0;
    tlv493d->m_base_x = 0;
    tlv493d->m_base_y = 0;
    tlv493d->m_base_z = 0;
    tlv493d->m_delta_x = 0;
    tlv493d->m_delta_y = 0;
    tlv493d->m_delta_z = 0;

    uint8_t buffer_read[10] = {0};
    uint8_t buffer_write[4] = {0};

    i2c_master_read_from_device(tlv493d->i2c_port, tlv493d->i2c_address, buffer_read, sizeof(buffer_read), I2C_MASTER_TIMEOUT_MS);

    buffer_write[0] = 0x00; 
    buffer_write[1] = (buffer_read[7] & 0x78) | 0x02; 
    buffer_write[2] = buffer_read[8]; 
    buffer_write[3] = buffer_read[9] & 0x7F; 
    
    if(tlv493d_calculate_even_parity(buffer_write, 4) == 0) buffer_write[3] |= 0x80; 

    i2c_master_write_to_device(tlv493d->i2c_port, tlv493d->i2c_address, buffer_write, sizeof(buffer_write), I2C_MASTER_TIMEOUT_MS);
}

void tlv493d_read(tlv493d_t *tlv493d) 
{
    uint8_t buffer_data[6] = {0};

    esp_err_t error = i2c_master_read_from_device(tlv493d->i2c_port, tlv493d->i2c_address, buffer_data, sizeof(buffer_data), I2C_MASTER_TIMEOUT_MS);
    
    if(error != ESP_OK) 
    {
        printf("I2C READ ERROR: %d\n", error); 
        return;
    }

    tlv493d->m_x = tlv493d_unpack_12bit(buffer_data[0], buffer_data[4] >> 4);
    tlv493d->m_y = tlv493d_unpack_12bit(buffer_data[1], buffer_data[4] & 0x0F);
    tlv493d->m_z = tlv493d_unpack_12bit(buffer_data[2], buffer_data[5] & 0x0F);

    tlv493d->m_delta_x = tlv493d->m_x - tlv493d->m_base_x;
    tlv493d->m_delta_y = tlv493d->m_y - tlv493d->m_base_y;
    tlv493d->m_delta_z = tlv493d->m_z - tlv493d->m_base_z;
}

void tlv493d_calibrate_center(tlv493d_t *tlv493d, uint32_t calibration_samples)
{
    int32_t sum_x = 0;
    int32_t sum_y = 0;
    int32_t sum_z = 0;

    for(uint32_t i = 0; i < calibration_samples; i++)
    {
        tlv493d_read_raw(tlv493d);

        sum_x += tlv493d->m_x;
        sum_y += tlv493d->m_y;
        sum_z += tlv493d->m_z;

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    int32_t average_x = sum_x / (int32_t)calibration_samples;
    int32_t average_y = sum_y / (int32_t)calibration_samples;
    int32_t average_z = sum_z / (int32_t)calibration_samples;

    tlv493d->m_base_x = (int16_t)average_x;
    tlv493d->m_base_y = (int16_t)average_y;
    tlv493d->m_base_z = (int16_t)average_z;
}

void tlv493d_get_decoupled_movement(tlv493d_t *tlv493d, float deadzone)
{
    float raw_delta_x = (float)tlv493d->m_delta_x;
    float raw_delta_y = (float)tlv493d->m_delta_y;
    float raw_delta_z = (float)tlv493d->m_delta_z;

    tlv493d->axis_x = (space_decoupling_matrix[0][0] * raw_delta_x) + (space_decoupling_matrix[0][1] * raw_delta_y) + (space_decoupling_matrix[0][2] * raw_delta_z);
    tlv493d->axis_y = (space_decoupling_matrix[1][0] * raw_delta_x) + (space_decoupling_matrix[1][1] * raw_delta_y) + (space_decoupling_matrix[1][2] * raw_delta_z);
    tlv493d->axis_z = (space_decoupling_matrix[2][0] * raw_delta_x) + (space_decoupling_matrix[2][1] * raw_delta_y) + (space_decoupling_matrix[2][2] * raw_delta_z);

    if(tlv493d->axis_z > 0.0f) tlv493d->axis_z *= TLV493D_UPWARDS_Z_GAIN;
    
    if(tlv493d->axis_x > -deadzone && tlv493d->axis_x < deadzone) tlv493d->axis_x = 0.0f;
    if(tlv493d->axis_y > -deadzone && tlv493d->axis_y < deadzone) tlv493d->axis_y = 0.0f;
    if(tlv493d->axis_z > -deadzone && tlv493d->axis_z < deadzone) tlv493d->axis_z = 0.0f;
}
