// Copyright 2020 DriveX.Tech. All rights reserved.
// 
// Licensed under the License.

#pragma once
#ifndef _UWB_TYPES_H_
#define _UWB_TYPES_H_

#include "types/types.h"

#ifdef __cplusplus
namespace dxt_common {
extern "C" {
#endif


typedef union _uwb_pan_id_t {
    UINT16 id2;
    UINT8 id1s[2];
} uwb_pan_id_t;

typedef UINT16 uwb_short_addr_t;
typedef UINT64 uwb_long_addr_t;
typedef union _uwb_addr_t {
    uwb_short_addr_t addr2;
    uwb_long_addr_t addr8;
    UINT8 addr1s[8];
} uwb_addr_t;

#ifdef __cplusplus
}
}
#endif

#endif