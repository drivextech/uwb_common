// Copyright 2020 DriveX.Tech. All rights reserved.
// 
// Licensed under the License.

#ifndef _MAC_FRAME_H_
#define _MAC_FRAME_H_

#include "types/types.h"
#include "types/uwb_types.h"



#ifdef UWB_PHY_EXTENDED
    #define UWB_FRMAE_MAX_LENGTH 1023
#else
    #define UWB_FRMAE_MAX_LENGTH (128 - 1)
#endif


typedef union _mac_frame_control_t {
    struct  {
        UINT8 frame_type: 3;
        UINT8 security_enabled_flag: 1;
        UINT8 frame_pending_flag: 1;
        UINT8 ack_request_flag: 1;
        UINT8 pan_id_compression_flag: 1;
        UINT8 unused: 3;
        UINT8 dest_addr_model: 2;
        UINT8 frame_version: 2;
        UINT8 src_addr_model: 2;
    } fcbs;
    UINT16 fc2;
} mac_frame_control_t;

typedef struct _mac_frame_header_t {
    mac_frame_control_t frame_control;
    UINT8 sequence_number;
    uwb_pan_id_t dest_pan_id;
    uwb_addr_t dest_addr;
    uwb_pan_id_t src_pan_id;
    uwb_addr_t src_addr;
    /* Aux security header */
} mac_frame_header_t;

typedef struct _mac_frame_footer_t {
    UINT16 fcs;
} mac_frame_footer_t;

typedef struct _mac_frame_payload_t {
    int length;
    UINT8* data;
} mac_frame_payload_t;

typedef struct _mac_frame_t {
    mac_frame_header_t header;
    mac_frame_payload_t payload;
    mac_frame_footer_t footer;
} mac_frame_t;


UINT16 pack_mac_frame_control(mac_frame_control_t mac_frame_control);

mac_frame_control_t unpack_mac_frame_control(UINT16 flag);

int generate_to_bytes(mac_frame_t* ptr_mac_frame, UINT8 buf[], int n);

int parse_from_bytes(UINT8 buf[], int n, mac_frame_t* ptr_mac_frame);


void mac_frame_init_blink(mac_frame_t* ptr_mac_frame);
void mac_frame_init_beacon(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_pan_id_t* src_pan_id);
void mac_frame_init_data_global(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* src_pan_id, uwb_pan_id_t* dest_pan_id);
void mac_frame_init_data_local(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* pan_id);
void mac_frame_init_acknowledgement(mac_frame_t* ptr_mac_frame);
void mac_frame_init_command_global(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* src_pan_id, uwb_pan_id_t* dest_pan_id);
void mac_frame_init_command_local(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* pan_id);


#endif