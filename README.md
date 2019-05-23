# 小辉报时

辉光管时钟（自制，未完成）

## 模块示意图

分为三层，最下层用STMCUBEMX提供的HAL库，中间层是针对各个外设写的控制程序等，最上层是应用逻辑

```
APP 逻辑部分？
---
FUN| PWR  BTN  WIFIMDL  HV507  RGBLED  FLASH  MUSIC  TIME  ALARM
---
HAL| GPIO  SLEEP  SYSWPK  EXIT  SPI  TIM  QSPI  SAI  DMA  I2C
```

## PWR: 电源部分，包括板子上5V电压的输出使能，高电压的输出使能，还有MCU的休眠
PWR_5V_ON()
PWR_5V_OFF()
PWR_VPP_ON()
PWR_VPP_OFF()
PWR_SleepWFI()  goto sleep mode and wait for interrupt from system wake up.
//PWR_MeasureVBat() use adc vbat channel to measure the vbat voltage. 
//PWR_XXXX() why power up?

## BTN: two button: nrst & mode, nrst btn is controlled by PWR
BTN_Scan() check btn status every loop

## WIFI: a UART-WIFI-MDL ESP8266
WIFI_Init() init UART and some res
WIFI_ConnectSSID() connect to a ap
WIFI_SetMode()  set mode to ap mode?
WIFI_GetHTML(url)  get a html from url if is connected to internet
WIFI_AsTCPServer()  let WIFI MDL be a TCP Server and wait for connect.
?????

## HV507: dma with spi, need PWR_VPP_ON() and PWR_5V_ON()
HV507_Init()  init the gpios to a certain status
HV507_SendData(uint64_t)
HV507_Refresh()  LE ON LE OFF

## RGBLED: 该模块维护一个缓冲区，大小为3*6字节，表示各个小灯的rgb颜色值；ws2812采用归零码表示0、1，这个rgb的缓冲区要对应一个3*8*8的输出缓冲区，然后用tim的pwm和dma功能输出。
RGBLED_Init()
RGBLED_SendData()
RGBLED_SetColorXXX()
> https://blog.csdn.net/ben392797097/article/details/78075699

## FLASH: FLASH用于ISP，在系统更新，这部分逻辑还需详细设计

## RTC: manage the ds3231 with i2c
RTC_Init()
RTC_SetXXXX Properties

## TIME: manage the rtc and alarm ints and use WIFI MDL get internet time or get mobile phone time
TIME_Set()
TIME_Get()
ALARM_Set()  manage a alarm list, and set the recent alarm to the rtc
ALARM_Get()

原谅我的英语水平
forgive my poor ENGLISH
