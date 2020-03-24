/**
 * Copyright 2020 DriveX.Tech. All rights reserved.
 * 
 * Licensed under the License.
 */

#include "mac_frame.h"
#include <string.h>
#include <assert.h>


UINT16 pack_mac_frame_control(mac_frame_control_t mac_frame_control)
{
    UINT16 flag = 0;
    flag |= mac_frame_control.fcbs.frame_type;
    flag |= (mac_frame_control.fcbs.security_enabled_flag << 3);
    flag |= (mac_frame_control.fcbs.frame_pending_flag << 4);
    flag |= (mac_frame_control.fcbs.ack_request_flag << 5);
    flag |= (mac_frame_control.fcbs.pan_id_compression_flag << 6);
    flag |= (mac_frame_control.fcbs.unused << 7);
    flag |= (mac_frame_control.fcbs.dest_addr_model << 10);
    flag |= (mac_frame_control.fcbs.frame_version << 12);
    flag |= (mac_frame_control.fcbs.src_addr_model << 14);

    return flag;
}

mac_frame_control_t unpack_mac_frame_control(UINT16 flag)
{
    mac_frame_control_t mac_frame_control;
    mac_frame_control.fcbs.frame_type = (UINT8)flag;
    mac_frame_control.fcbs.security_enabled_flag = (flag >> 3);
    mac_frame_control.fcbs.frame_pending_flag = (flag >> 4);
    mac_frame_control.fcbs.ack_request_flag = (flag >> 5);
    mac_frame_control.fcbs.pan_id_compression_flag = (flag >> 6);
    mac_frame_control.fcbs.unused = (flag >> 7);
    mac_frame_control.fcbs.dest_addr_model = (flag >> 10);
    mac_frame_control.fcbs.frame_version = (flag >> 12);
    mac_frame_control.fcbs.src_addr_model = (flag >> 14);

    return mac_frame_control;
}

int generate_mac_frame_hdr_to_bytes(mac_frame_header_t* ptr_mac_frame_hdr, UINT8 buf[], size_t n)
{
    int hdr_len = 0;
    UINT8* p_buf_curpos = buf;

    mac_frame_control_t mac_frame_control = ptr_mac_frame_hdr->frame_control;

    UINT16* ptr_mfc = (UINT16*)p_buf_curpos;
    *ptr_mfc = pack_mac_frame_control(mac_frame_control);
    hdr_len += 2;
    p_buf_curpos += 2;
    assert(hdr_len <= n);

    buf[hdr_len++] = ptr_mac_frame_hdr->sequence_number;
    ++p_buf_curpos;
    assert(hdr_len <= n);
    
    if(mac_frame_control.fcbs.dest_addr_model == 2 // (b1, b0): (1,0)
        || mac_frame_control.fcbs.dest_addr_model == 3 // (b1, b0): (1,1)
    ) {
        UINT16* ptr_dest_pan_id = (UINT16*)p_buf_curpos;
        *ptr_dest_pan_id = ptr_mac_frame_hdr->dest_pan_id.id2;
        hdr_len += 2;
        p_buf_curpos += 2;
        assert(hdr_len <= n);

        if(mac_frame_control.fcbs.dest_addr_model == 2) {
            UINT16* ptr_dest_addr = (UINT16*)p_buf_curpos;
            *ptr_dest_addr = ptr_mac_frame_hdr->dest_addr.addr2;
            hdr_len += 2;
            p_buf_curpos += 2;
            assert(hdr_len <= n);
        }
        if(mac_frame_control.fcbs.dest_addr_model == 3) {
            UINT64* ptr_dest_addr = (UINT64*)p_buf_curpos;
            *ptr_dest_addr = ptr_mac_frame_hdr->dest_addr.addr8;
            hdr_len += 8;
            p_buf_curpos += 8;
            assert(hdr_len <= n);
        }
    } else if(mac_frame_control.fcbs.dest_addr_model == 0) { // (b1, b0): (0,0)
    } else { // (b1, b0): (0,1), reserved
    }
    assert(hdr_len <= n);

    if(mac_frame_control.fcbs.pan_id_compression_flag) {
        // skip src_pan_id field
    } else {
        if(mac_frame_control.fcbs.src_addr_model == 2 // (b1, b0): (1,0)
            || mac_frame_control.fcbs.src_addr_model == 3 // (b1, b0): (1,1)
        ) {
            UINT16* ptr_src_pan_id = (UINT16*)p_buf_curpos;
            *ptr_src_pan_id = ptr_mac_frame_hdr->src_pan_id.id2;
            hdr_len += 2;
            p_buf_curpos += 2;
            assert(hdr_len <= n);
        } else if(mac_frame_control.fcbs.src_addr_model == 0) { // (b1, b0): (0,0)
        } else { // (b1, b0): (0,1), reserved
        }
    }
    if(mac_frame_control.fcbs.src_addr_model == 2) { // (b1, b0): (1,0)
        UINT16* ptr_src_addr = (UINT16*)p_buf_curpos;
        *ptr_src_addr = ptr_mac_frame_hdr->src_addr.addr2;
        hdr_len += 2;
        p_buf_curpos += 2;
        assert(hdr_len <= n);
    } else if(mac_frame_control.fcbs.src_addr_model == 3) { // (b1, b0): (1,1)
        UINT64* ptr_src_addr = (UINT64*)p_buf_curpos;
        *ptr_src_addr = ptr_mac_frame_hdr->src_addr.addr8;
        hdr_len += 8;
        p_buf_curpos += 8;
        assert(hdr_len <= n);
    } else if(mac_frame_control.fcbs.src_addr_model == 0) { // (b1, b0): (0,0)
    } else { // (b1, b0): (0,1), reserved
    }
    assert(hdr_len <= n);

    if(mac_frame_control.fcbs.security_enabled_flag) {
        // TODO
    }

    return hdr_len;
}

int parse_mac_frame_hdr_from_bytes(UINT8 buf[], size_t n, mac_frame_header_t* ptr_mac_frame_hdr)
{
    int hdr_len = 0;
    UINT8* p_buf_curpos = buf;

    mac_frame_control_t mac_frame_control;
    UINT16* ptr_mfc = (UINT16*)p_buf_curpos;
    mac_frame_control = unpack_mac_frame_control(*ptr_mfc);
    ptr_mac_frame_hdr->frame_control = mac_frame_control;
    hdr_len += 2;
    p_buf_curpos += 2;
    assert(hdr_len <= n);

    ptr_mac_frame_hdr->sequence_number = buf[hdr_len++];
    ++p_buf_curpos;
    assert(hdr_len <= n);
    
    if(mac_frame_control.fcbs.dest_addr_model == 2 // (b1, b0): (1,0)
        || mac_frame_control.fcbs.dest_addr_model == 3 // (b1, b0): (1,1)
    ) {
        UINT16* ptr_dest_pan_id = (UINT16*)p_buf_curpos;
        ptr_mac_frame_hdr->dest_pan_id.id2 = *ptr_dest_pan_id;
        hdr_len += 2;
        p_buf_curpos += 2;
        assert(hdr_len <= n);

        if(mac_frame_control.fcbs.dest_addr_model == 2) {
            UINT16* ptr_dest_addr = (UINT16*)p_buf_curpos;
            ptr_mac_frame_hdr->dest_addr.addr2 = *ptr_dest_addr;
            hdr_len += 2;
            p_buf_curpos += 2;
            assert(hdr_len <= n);
        }
        if(mac_frame_control.fcbs.dest_addr_model == 3) {
            UINT64* ptr_dest_addr = (UINT64*)p_buf_curpos;
            ptr_mac_frame_hdr->dest_addr.addr8 = *ptr_dest_addr;
            hdr_len += 8;
            p_buf_curpos += 8;
            assert(hdr_len <= n);
        }
    } else if(mac_frame_control.fcbs.dest_addr_model == 0) { // (b1, b0): (0,0)
    } else { // (b1, b0): (0,1), reserved
    }
    assert(hdr_len <= n);

    if(mac_frame_control.fcbs.pan_id_compression_flag) {
        // src_pan_id equals dest_pan_id
        ptr_mac_frame_hdr->src_pan_id = ptr_mac_frame_hdr->dest_pan_id;
    } else {
        if(mac_frame_control.fcbs.src_addr_model == 2 // (b1, b0): (1,0)
            || mac_frame_control.fcbs.src_addr_model == 3 // (b1, b0): (1,1)
        ) {
            UINT16* ptr_src_pan_id = (UINT16*)p_buf_curpos;
            ptr_mac_frame_hdr->src_pan_id.id2 = *ptr_src_pan_id;
            hdr_len += 2;
            p_buf_curpos += 2;
            assert(hdr_len <= n);
        } else if(mac_frame_control.fcbs.src_addr_model == 0) { // (b1, b0): (0,0)
        } else { // (b1, b0): (0,1), reserved
        }
    }
    if(mac_frame_control.fcbs.src_addr_model == 2) { // (b1, b0): (1,0)
        UINT16* ptr_src_addr = (UINT16*)p_buf_curpos;
        ptr_mac_frame_hdr->src_addr.addr2 = *ptr_src_addr;
        hdr_len += 2;
        p_buf_curpos += 2;
        assert(hdr_len <= n);
    } else if(mac_frame_control.fcbs.src_addr_model == 3) { // (b1, b0): (1,1)
        UINT64* ptr_src_addr = (UINT64*)p_buf_curpos;
        ptr_mac_frame_hdr->src_addr.addr8 = *ptr_src_addr;
        hdr_len += 8;
        p_buf_curpos += 8;
        assert(hdr_len <= n);
    } else if(mac_frame_control.fcbs.src_addr_model == 0) { // (b1, b0): (0,0)
    } else { // (b1, b0): (0,1), reserved
    }
    assert(hdr_len <= n);

    if(mac_frame_control.fcbs.security_enabled_flag) {
        // TODO
    }

    return hdr_len;
}

int generate_mac_frame_ftr_to_bytes(mac_frame_footer_t* ptr_mac_frame_ftr, UINT8 buf[], size_t n)
{
    assert(n >= 2);

    // TODO
    buf[0] = 0;
    buf[1] = 0;
    //buf[0] = ptr_mac_frame_ftr->fcs;
    //buf[0] = ptr_mac_frame_ftr->fcs >> 8;

    return 2;
}

int parse_mac_frame_ftr_from_bytes(UINT8 buf[], size_t n, mac_frame_footer_t* ptr_mac_frame_ftr)
{
    assert(n >= 2);

    UINT16* p = (UINT16*)buf;
    ptr_mac_frame_ftr->fcs = *p;

    return 2;
}

int generate_mac_frame_payload_to_bytes(mac_frame_payload_t* ptr_mac_frame_payload, UINT8 buf[], size_t n)
{
    if(ptr_mac_frame_payload->length > 0) {
        memcpy(buf, ptr_mac_frame_payload->data, ptr_mac_frame_payload->length);
    }

    return ptr_mac_frame_payload->length;
}

int parse_mac_frame_payload_from_bytes(UINT8 buf[], int n, mac_frame_payload_t* ptr_mac_frame_payload)
{
    ptr_mac_frame_payload->length = n;
    if(n > 0) {
        memcpy(ptr_mac_frame_payload->data, buf, n);
    }

    return ptr_mac_frame_payload->length;
}


int generate_to_bytes(mac_frame_t* ptr_mac_frame, UINT8 buf[], int n)
{
    assert(ptr_mac_frame!=NULL && n > 0);

    int frame_len = 0;

    mac_frame_header_t* ptr_mac_frame_hdr = &ptr_mac_frame->header;
    int hdr_len = generate_mac_frame_hdr_to_bytes(ptr_mac_frame_hdr, buf, n);
    frame_len += hdr_len;
    assert(frame_len <= n);

    mac_frame_payload_t* ptr_mac_frame_payload = &ptr_mac_frame->payload;
    int payload_len = generate_mac_frame_payload_to_bytes(ptr_mac_frame_payload, buf + frame_len, n - frame_len);
    frame_len += payload_len;
    assert(frame_len <= n);
    
    mac_frame_footer_t* ptr_mac_frame_ftr = &ptr_mac_frame->footer;
    int ftr_len = generate_mac_frame_ftr_to_bytes(ptr_mac_frame_ftr, buf + frame_len, n - frame_len);
    frame_len += ftr_len;
    assert(frame_len <= n);

    return frame_len;
}

int parse_from_bytes(UINT8 buf[], int n, mac_frame_t* ptr_mac_frame)
{
    assert(n > 0 && ptr_mac_frame!=NULL);

    int frame_len = 0;

    mac_frame_header_t* ptr_mac_frame_hdr = &ptr_mac_frame->header;
    int hdr_len = parse_mac_frame_hdr_from_bytes(buf, n, ptr_mac_frame_hdr);
    frame_len += hdr_len;
    assert(frame_len <= n);

    mac_frame_footer_t* ptr_mac_frame_ftr = &ptr_mac_frame->footer;
    int ftr_len = parse_mac_frame_ftr_from_bytes(buf + n - 2, 2, ptr_mac_frame_ftr);
    frame_len += ftr_len;
    assert(frame_len <= n);

    mac_frame_payload_t* ptr_mac_frame_payload = &ptr_mac_frame->payload;
    int payload_len = parse_mac_frame_payload_from_bytes(buf + hdr_len, n - hdr_len - ftr_len, ptr_mac_frame_payload);
    frame_len += payload_len;
    assert(frame_len <= n);

    return frame_len;
}


void mac_frame_init_blink(mac_frame_t* ptr_mac_frame)
{
    assert(0 && "Not Impl!");
}

void mac_frame_init_beacon(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_pan_id_t* src_pan_id)
{
    assert(ptr_mac_frame != NULL);

    ptr_mac_frame->header.frame_control.fc2 = 0;
    ptr_mac_frame->header.frame_control.fcbs.frame_type = 0; // beacon type
    ptr_mac_frame->header.frame_control.fcbs.frame_version = 1;
    ptr_mac_frame->header.frame_control.fcbs.security_enabled_flag = 0; // security enable bit, 0 disable, 1 enable
    ptr_mac_frame->header.frame_control.fcbs.src_addr_model = 2; // short addr
    ptr_mac_frame->header.frame_control.fcbs.frame_pending_flag = 0; // if broadcast data or command frame is pending, set 1
    ptr_mac_frame->header.src_addr = *src_addr;
    ptr_mac_frame->header.src_pan_id = *src_pan_id;

    if(ptr_mac_frame->header.frame_control.fcbs.security_enabled_flag) {
        //TODO: security related
    }

    //TODO: superframe, GTS, Pending address, setting.
}

void mac_frame_init_data_global(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* src_pan_id, uwb_pan_id_t* dest_pan_id)
{
    assert(ptr_mac_frame != NULL);

    ptr_mac_frame->header.frame_control.fc2 = 0;
    ptr_mac_frame->header.frame_control.fcbs.frame_type = 1; // data type
    ptr_mac_frame->header.frame_control.fcbs.frame_version = 1;
    ptr_mac_frame->header.frame_control.fcbs.security_enabled_flag = 0; // security enable bit, 0 disable, 1 enable
    ptr_mac_frame->header.frame_control.fcbs.frame_pending_flag = 0; // if broadcast data or command frame is pending, set 1
    ptr_mac_frame->header.frame_control.fcbs.src_addr_model = 2; // short addr
    ptr_mac_frame->header.frame_control.fcbs.dest_addr_model = 2; // short addr
    ptr_mac_frame->header.src_addr = *src_addr;
    ptr_mac_frame->header.dest_addr = *dest_addr;
    ptr_mac_frame->header.src_pan_id = *src_pan_id;
    ptr_mac_frame->header.dest_pan_id = *dest_pan_id;

    if(ptr_mac_frame->header.frame_control.fcbs.security_enabled_flag) {
        //TODO: security related
    }
}
void mac_frame_init_data_local(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* pan_id)
{
    assert(ptr_mac_frame != NULL);

    mac_frame_init_data_global(ptr_mac_frame, src_addr, dest_addr, pan_id, pan_id);
    ptr_mac_frame->header.frame_control.fcbs.pan_id_compression_flag = 1;
}

void mac_frame_init_acknowledgement(mac_frame_t* ptr_mac_frame)
{
    assert(ptr_mac_frame != NULL);

    ptr_mac_frame->header.frame_control.fc2 = 0;
    ptr_mac_frame->header.frame_control.fcbs.frame_type = 2; // acknowledgement type
    ptr_mac_frame->header.frame_control.fcbs.frame_version = 1;
    ptr_mac_frame->header.frame_control.fcbs.frame_pending_flag = 0; // if data is pending, set 1
}

void mac_frame_init_command_global(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* src_pan_id, uwb_pan_id_t* dest_pan_id)
{
    assert(ptr_mac_frame != NULL);

    ptr_mac_frame->header.frame_control.fc2 = 0;
    ptr_mac_frame->header.frame_control.fcbs.frame_type = 3; // command type
    ptr_mac_frame->header.frame_control.fcbs.frame_version = 1;
    ptr_mac_frame->header.frame_control.fcbs.security_enabled_flag = 0; // security enable bit, 0 disable, 1 enable
    ptr_mac_frame->header.frame_control.fcbs.frame_pending_flag = 0; // if broadcast data or command frame is pending, set 1
    ptr_mac_frame->header.frame_control.fcbs.src_addr_model = 2; // short addr
    ptr_mac_frame->header.frame_control.fcbs.dest_addr_model = 2; // short addr
    ptr_mac_frame->header.src_addr = *src_addr;
    ptr_mac_frame->header.dest_addr = *dest_addr;
    ptr_mac_frame->header.src_pan_id = *src_pan_id;
    ptr_mac_frame->header.dest_pan_id = *dest_pan_id;

    if(ptr_mac_frame->header.frame_control.fcbs.security_enabled_flag) {
        //TODO: security related
    }

    //TODO: command frame identifier
}
void mac_frame_init_command_local(mac_frame_t* ptr_mac_frame, uwb_addr_t* src_addr, uwb_addr_t* dest_addr, uwb_pan_id_t* pan_id)
{
    assert(ptr_mac_frame != NULL);

    mac_frame_init_command_global(ptr_mac_frame, src_addr, dest_addr, pan_id, pan_id);
    ptr_mac_frame->header.frame_control.fcbs.pan_id_compression_flag = 1;
}