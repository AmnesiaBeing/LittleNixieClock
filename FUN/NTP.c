#include "FUN/NTP.h"
#include "FUN/Time.h"

#include "DRV/DS3231.h"

#include "FUN/Network.h"

#include <string.h>
#include <stddef.h>

#define NTP_TIMESTAMP_DELTA 2208988800ull

#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 0
#define PREC 0

bool NTP_GetTime(time_t *t)
{
    NTP_Package_t packet = {0};
    memset(&packet, 0, sizeof(NTP_Package_t));
    packet.li_vn_mode_stratum_poll_precision = BigLittleSwap32((LI << 30) | (VN << 27) | (MODE << 24) | (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));

    LinkID id = Network_ConnectBlock(TCPIP_UDP, "ntp1.aliyun.com", 123);

    if (id == ERROR_LINKID)
    {
        return false;
    }

    if (!Network_WriteBlock(id, (uint8_t *)&packet, sizeof(NTP_Package_t)))
    {
        return false;
    }

    size_t len = 0;

    if (!Network_ReadBlock(id, (uint8_t *)&packet, &len, 2000))
    {
        return false;
    }

    if (len != sizeof(NTP_Package_t))
    {
        return false;
    }

    packet.txTm_s = BigLittleSwap32(packet.txTm_s); // Time-stamp seconds.

    *t = (time_t)(packet.txTm_s - NTP_TIMESTAMP_DELTA);

    return true;
}