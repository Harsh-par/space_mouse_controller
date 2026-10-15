#include "space_mouse_tinyusb/usb_hid.h"
#include "space_mouse_tinyusb/usb_descriptors.h"

#include "tinyusb.h"
#include "tinyusb_default_config.h"
#include "class/hid/hid_device.h"

void usb_hid_init(void) 
{
    tinyusb_config_t tusb_configuration = TINYUSB_DEFAULT_CONFIG();

    tusb_configuration.descriptor.device            = &tusb_descriptor_device;
    tusb_configuration.descriptor.string            = (const char **)tusb_descriptor_string;
    tusb_configuration.descriptor.string_count      = TUSB_DESCRIPTOR_STRING_COUNT;
    tusb_configuration.descriptor.full_speed_config = tusb_descriptor_configuration;

    tinyusb_driver_install(&tusb_configuration);
}

void usb_hid_send_mouse(uint8_t buttons, int8_t x, int8_t y, int8_t scroll) 
{
    if(tud_hid_ready()) 
    {
        tud_hid_mouse_report(REPORT_ID_MOUSE, buttons, x, y, scroll, 0);
    }
}

void usb_hid_send_keyboard(uint8_t keycode, uint8_t modifier) 
{
    if(tud_hid_ready()) 
    {
        uint8_t keycodes[6] = {keycode, 0, 0, 0, 0, 0};
        tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycodes);
    }
}

void usb_hid_send_spacemouse(space_mouse_data_t *space_mouse_data)
{
    if(!tud_hid_ready()) return;

    static space_mouse_state_t space_mouse_state = SPACE_MOUSE_STATE_TRANSLATION;

    switch(space_mouse_state)
    {
        case SPACE_MOUSE_STATE_TRANSLATION: 
        {
            int16_t translation[3] = {
                space_mouse_data->x,
                space_mouse_data->y,
                space_mouse_data->z
            };

            if(tud_hid_report(REPORT_ID_TRANSLATION, translation, sizeof(translation)))
            {
                space_mouse_state = SPACE_MOUSE_STATE_ROTATION;
            }
            break;
        }
        case SPACE_MOUSE_STATE_ROTATION:
        {
            int16_t rotation[3] = {
                space_mouse_data->rx,
                space_mouse_data->ry,
                space_mouse_data->rz
            };

            if(tud_hid_report(REPORT_ID_ROTATION, rotation, sizeof(rotation)))
            {
                space_mouse_state = SPACE_MOUSE_STATE_TRANSLATION;
            }
            break;
        }
        default: break;
    }
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen) 
{
    return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize) 
{

}