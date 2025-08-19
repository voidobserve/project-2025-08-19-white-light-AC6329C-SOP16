/*********************************************************************************************
    *   Filename        : sm.h

    *   Description     : 

    *   Author          : Minxian

    *   Email           : liminxian@zh-jieli.com

    *   Last modifiled  : 2020-07-01 16:36

    *   Copyright:(c)JIELI  2011-2020  @ , All Rights Reserved.
*********************************************************************************************/
#ifndef _BT_SM_H_
#define _BT_SM_H_

#include "typedef.h"

// IO Capability Values
typedef enum {
    IO_CAPABILITY_DISPLAY_ONLY = 0,
    IO_CAPABILITY_DISPLAY_YES_NO,
    IO_CAPABILITY_KEYBOARD_ONLY,
    IO_CAPABILITY_NO_INPUT_NO_OUTPUT,
    IO_CAPABILITY_KEYBOARD_DISPLAY, // not used by secure simple pairing
} io_capability_t;

void ble_sm_setup_init(io_capability_t io_type, u8 auth_req, uint8_t min_key_size, u8 security_en);

void ble_cbk_handler_register(btstack_packet_handler_t packet_cbk, sm_stack_packet_handler_t sm_cbk);

void ble_sm_setup_init(io_capability_t io_type, u8 auth_req, uint8_t min_key_size, u8 security_en);

void sm_just_works_confirm(hci_con_handle_t con_handle);

void sm_init(void);

void sm_set_io_capabilities(io_capability_t io_capability);

void sm_set_authentication_requirements(uint8_t auth_req);

void sm_set_encryption_key_size_range(uint8_t min_size, uint8_t max_size);

void sm_set_request_security(int enable);

void sm_event_callback_set(void(*cbk_sm_ph)(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size));

#endif
