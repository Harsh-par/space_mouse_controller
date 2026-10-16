#ifndef SPACE_MOUSE_H
#define SPACE_MOUSE_H

#include <math.h>

#include "tlv493d.h"

#include "space_mouse_input/mouse_switch.h"
#include "space_mouse_input/key_switch.h"
#include "space_mouse_input/rotary_encoder.h"

#include "tinyusb.h"
#include "class/hid/hid.h"

void space_mouse_initialize_magnetometer(tlv493d_t *tlv493d);
void space_mouse_get_decoupled_movement(tlv493d_t *tlv493d, float deadzone);

void space_mouse_handle_usb_hid(tlv493d_t *tlv493d, mouse_switch_t *mouse_switches, key_switch_t *key_switches, rotary_encoder_t *rotary_encoder);

void space_mouse_print_direction(tlv493d_t *tlv493d);
void space_mouse_print_axis(tlv493d_t *tlv493d);
void space_mouse_print_deltas(tlv493d_t *tlv493d);

#endif