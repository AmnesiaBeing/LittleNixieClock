## 模块示意图

```
APP
---
FUN| PWR  BTN  WIFIMDL  HV507  RGBLED  FLASH  MUSIC  TIME  ALARM
---
HAL| GPIO  SLEEP  SYSWPK  EXIT  SPI  TIM  QSPI  SAI  DMA  I2C
```

PWR:
PWR_5V_ON()
PWR_5V_OFF()
PWR_VPP_ON()
PWR_VPP_OFF()
PWR_SleepWFI()  goto sleep mode and wait for interrupt from system wake up.
PWR_MeasureVBat() use adc vbat channel to measure the vbat voltage.

PWR_XXXX() why power up?

BTN: two button: nrst & mode, nrst btn is controlled by PWR
BTN_Scan() check btn status every loop

WIFI: a UART-WIFI-MDL ESP8266
WIFI_Init() init UART and some res
WIFI_ConnectSSID() connect to a ap
WIFI_SetMode()  set mode to ap mode?
WIFI_GetHTML(url)  get a html from url if is connected to internet
WIFI_AsTCPServer()  let WIFI MDL be a TCP Server and wait for connect.
?????

HV507: dma with spi, need PWR_VPP_ON() and PWR_5V_ON()
HV507_Init()  init the gpios to a certain status
HV507_SendData(uint64_t)
HV507_Refresh()  LE ON LE OFF

RGBLED: dma with tim2_ch1
RGBLED_Init()
RGBLED_SendData()

FLASH: i haven't used it perhaps save the configurations

RTC: manage the ds3231 with i2c
RTC_Init()
RTC_SetXXXX Properties

TIME: manage the rtc and alarm ints and use WIFI MDL get internet time or get mobile phone time
TIME_Set()
TIME_Get()
ALARM_Set()  manage a alarm list, and set the recent alarm to the rtc
ALARM_Get()

forgive my poor ENGLISH