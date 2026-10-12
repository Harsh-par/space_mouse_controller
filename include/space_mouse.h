#ifndef SPACE_MOUSE_H
#define SPACE_MOUSE_H

#include <math.h>

#include "tlv493d.h"

void space_mouse_initialize_magnetometer(tlv493d_t *tlv493d);
void space_mouse_get_decoupled_movement(tlv493d_t *tlv493d, float deadzone);
void space_mouse_print_direction(tlv493d_t *tlv493d);
void space_mouse_print_axis(tlv493d_t *tlv493d);
void space_mouse_print_deltas(tlv493d_t *tlv493d);

#endif