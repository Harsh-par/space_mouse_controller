#include "space_mouse.h"

static const float space_decoupling_matrix[3][3] = {
    {    0.0859f,    -2.4073f,     0.4259f},
    {    2.7996f,     0.1626f,     0.3729f},
    {   -0.0406f,     0.1858f,     0.9977f}
};

void space_mouse_get_decoupled_movement(tlv493d_t *tlv493d, float deadzone)
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