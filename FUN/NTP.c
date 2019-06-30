#include "FUN/NTP.h"
#include "FUN/Time.h"

// ?代码结构有点难
#include "DRV/ESP8266/Wifi.h"

#include "FUN/NixieTube.h"
#include "DRV/DS3231.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>
// 两个部分，一个部分负责发送，并且有个回调函数，来更改DS3231中的时间
#define NTP_TIMESTAMP_DELTA 2208988800ull
// 发送部分

#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 0
#define PREC 0

bool NTP_GetTimeSend()
{
    NTP_Package_t packet = {0};
    memset(&packet, 0, sizeof(NTP_Package_t));
    packet.li_vn_mode_stratum_poll_precision = BigLittleSwap32((LI << 30) | (VN << 27) | (MODE << 24) | (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));

    return Wifi_TcpIp_SendDataUdp(0, sizeof(NTP_Package_t), (uint8_t *)&packet);
}

bool NTP_GetTimeRecv(uint8_t *data, uint16_t len)
{
    NTP_Package_t *packet = (NTP_Package_t *)data;

    packet->txTm_s = BigLittleSwap32(packet->txTm_s); // Time-stamp seconds.

    time_t txTm = (time_t)(packet->txTm_s - NTP_TIMESTAMP_DELTA);
    // printf("Time: %s", ctime((const time_t *)&txTm));

    // UTC+8
    txTm += 8 * 60 * 60;

    DS3231_SetTime(gmtime((const time_t *)&txTm));

    // char *str = ctime((const time_t *)&txTm);
    // char *t;
    // t = strtok(str, " ");
    // t = strtok(NULL, " ");
    // t = strtok(NULL, " ");
    // t = strtok(NULL, " ");
    // NixieTube_Show(t);

    return true;
}