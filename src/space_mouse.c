#include "space_mouse.h"

static const float space_decoupling_matrix[3][3] = {
    {    0.0859f,    -2.4073f,     0.4259f},
    {    2.7996f,     0.1626f,     0.3729f},
    {   -0.0406f,     0.1858f,     0.9977f}
};

void space_mouse_initialize_magnetometer(tlv493d_t *tlv493d)
{
    tlv493d_init_i2c(tlv493d, I2C_MASTER_SDA_GPIO, I2C_MASTER_SCL_GPIO, I2C_MASTER_PORT, I2C_ADDRESS_TLV493D, true);
    tlv493d_init_device(tlv493d);
    tlv493d_calibrate(tlv493d, TLV493D_CALIBRATION_SAMPLES);
}

void space_mouse_get_decoupled_movement(tlv493d_t *tlv493d, float deadzone)
{
    tlv493d_read(tlv493d);

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

void space_mouse_print_direction(tlv493d_t *tlv493d)
{
    float abs_x = fabsf(tlv493d->axis_x);
    float abs_y = fabsf(tlv493d->axis_y);
    float abs_z = fabsf(tlv493d->axis_z);

    const char *direction = "IDLE";

    if(abs_x > 0.0f || abs_y > 0.0f || abs_z > 0.0f) 
    {
        if(abs_x >= abs_y && abs_x >= abs_z) 
        {
            direction = (tlv493d->axis_x > 0.0f) ? "RIGHT" : "LEFT";
        } 
        else if(abs_y >= abs_x && abs_y >= abs_z) 
        {
            direction = (tlv493d->axis_y > 0.0f) ? "FORWARD" : "BACKWARD";
        } 
        else if(abs_z > abs_x && abs_z > abs_y)
        {
            direction = (tlv493d->axis_z < 0.0f) ? "PUSH DOWN" : "PULL UP";
        }
    }     

    printf("DIR %-10s\n", direction);
}

void space_mouse_print_axis(tlv493d_t *tlv493d)
{
    printf("AXIS %d %d %d\n", abs(tlv493d->axis_x), abs(tlv493d->axis_y), abs(tlv493d->axis_z));
}

void space_mouse_print_deltas(tlv493d_t *tlv493d)
{
    printf("DELTA %d %d %d\n", tlv493d->m_delta_x, tlv493d->m_delta_y, tlv493d->m_delta_z);
}