#pragma once
// Structure that defines the 48 byte NTP packet protocol.

#include <stdint.h>

typedef struct
{
    uint32_t li_vn_mode_stratum_poll_precision; // li.   Two bits.   Leap indicator.
                                                // vn.   Three bits. Version number of the protocol.
                                                // mode. Three bits. Client will pick mode 3 for client.
                                                // Eight bits. Stratum level of the local clock.
                                                // Eight bits. Maximum interval between successive messages.
                                                // Eight bits. Precision of the local clock.

    uint32_t rootDelay;      // 32 bits. Total round trip delay time.
    uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
    uint32_t refId;          // 32 bits. Reference clock identifier.

    uint32_t refTm_s; // 32 bits. Reference time-stamp seconds.
    uint32_t refTm_f; // 32 bits. Reference time-stamp fraction of a second.

    uint32_t origTm_s; // 32 bits. Originate time-stamp seconds.
    uint32_t origTm_f; // 32 bits. Originate time-stamp fraction of a second.

    uint32_t rxTm_s; // 32 bits. Received time-stamp seconds.
    uint32_t rxTm_f; // 32 bits. Received time-stamp fraction of a second.

    uint32_t txTm_s; // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
    uint32_t txTm_f; // 32 bits. Transmit time-stamp fraction of a second.
} NTP_Package_t;     // Total: 384 bits or 48 bytes.

#define BigLittleSwap32(A) ((((uint32_t)(A)&0xff000000) >> 24) | \
                            (((uint32_t)(A)&0x00ff0000) >> 8) |  \
                            (((uint32_t)(A)&0x0000ff00) << 8) |  \
                            (((uint32_t)(A)&0x000000ff) << 24))
