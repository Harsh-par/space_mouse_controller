#ifndef SPACE_MOUSE_H
#define SPACE_MOUSE_H

#include "tlv493d.h"

void space_mouse_initialize_magnetometer(tlv493d_t *tlv493d);
void space_mouse_get_decoupled_movement(tlv493d_t *tlv493d, float deadzone);

#endif