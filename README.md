# 小辉报时

辉光管时钟（自制，未完成）

## 模块示意图

分为三层，最下层用STMCUBEMX提供的HAL库，中间层是针对各个外设写的控制程序等，最上层是应用逻辑

```
APP 逻辑部分？
---
FUN| PWR  BTN  WIFI NixieTube RGBLED  FLASH  MUSIC TIME  ALARM
___
DRV| ESP8266 HV507 DS3231 WS2812
---
HAL| GPIO  SLEEP  SYSWPK  EXIT  SPI  TIM  QSPI  SAI  DMA  I2C
```

## PWR: 

电源部分，包括板子上5V电压的输出使能，高电压的输出使能，还有MCU的休眠

* PWR_5V_ON()
* PWR_5V_OFF()
* PWR_VPP_ON()
* PWR_VPP_OFF()
* PWR_SleepWFI()  goto sleep mode and wait for interrupt from system wake up.
* PWR_MeasureVBat() 这是个测试点，如果stm32的vbat不接vcc，甚至悬空，会有何种后果？ 
//PWR_XXXX() why power up?

## BTN: two button: nrst & mode, nrst btn is controlled by PWR

* BTN_Scan() 状态机，每个循环都判断一次IO口状态，如果按钮按下，判断短按还是长按，执行不同的功能（功能未定）

## WIFI:

ESP8266模块是一个透传模块。提供调用ESP8266 wifi模块的一些方法，比如建立tcpserver什么的。

* WIFI_Init() 初始化ESP8266驱动
* WIFI_ConnectSSID() 让WIFI模块连接一个热点
* WIFI_SetMode()  让WIFI模块作为一个热点
* WIFI_GetHTML(url)  get a html from url if is connected to internet
* WIFI_AsTCPServer()  在局域网中，作为一个TCPServer，监听连接，在主循环中返回连接成功，只能接受一个传入的连接
* WIFI_TCPConnect(ip,port) 在局域网中，主动连接
* WIFI_MainLoop() 处理事务
* WIFI_SendData(ptr,len)

## HV507:

HV507是一个串并转换的芯片，这里使用SPI作为主机只发送的功能，还用到了DMA。

* HV507_Init()  让IO口处于一个确定的状态
* HV507_SendData(uint64_t) 通过HAL_SPI_Transmit_DMA()发送数据。
* HV507_Refresh() 让LE产生一个脉冲，以刷新数据显示。这里如果能够找到在上述DMA发送完成的中断后延时产生脉冲的方法就不需要这个方法了。

别人都用的HV57708芯片，这个芯片是自己选择的，感觉在软件上可以直接使用SPI功能，而不是单独控制每一个引脚的输出。

## NixieTube：

一共6个辉光管和4个氖管。这个模块负责维护一个显示缓冲区，分别表示这些管子的状态，并提供一些好玩的显示函数。

辉光管的作用主要是显示数字，这里大概思路是把输入的字符串转换为hv507能够输出的东西。

* NT_???

## RGBLED: 

一共6个WS2812小灯。该模块维护一个缓冲区，大小为3×6字节，表示各个小灯的grb颜色值。

* RGBLED_Init()
* RGBLED_SetColorXXX()
* RGBLED_SendData()  转换成下面需要的输出，并调用

## WS2812:

ws2812采用归零码表示0、1，该模块维护一个3×8×8的输出缓冲区，然后用tim的pwm和dma功能输出。

* WS2812_SendData(uint8_t*[3*8*8]+3+1)

> https://blog.csdn.net/ben392797097/article/details/78075699

## FLASH:

FLASH用于ISP（在系统更新）和存储配置信息，这部分逻辑还需详细设计

* FLASH_Init()?

## DS3231:

DS3231是一个RTC芯片，通过I2C协议通信，这个模块用于配置寄存器。

* RTC_Init()
* RTC_Set/GetXXXX() 设置/获取属性

## TIME&AlARM: 

管理单片机对时间认知的模块，维护一系列闹钟的模块。

可以认为，在1s内，单片机的晶振还是准确的。需要在合适的时候通过WIFI模块或者RTC模块更新时间

不妨设定，闹钟最多5个，而且可以进行设定。在硬件上，RTC芯片只支持两个闹钟，这时可能需要维护一个列表，将最近的闹钟给写到RTC里，等待芯片被唤醒。

* TIME_Set()
* TIME_Get()
* ALARM_Set()  manage a alarm list, and set the recent alarm to the rtc
* ALARM_Get()

## ISP:

提供对程序的在系统更新功能。

## 主程序逻辑

总的思路，这只是一个摆放装置，

第一次启动->显示开机动画->读取配置信息，初始化WIFI模块，作为热点等待连接或者主动连接别的热点，联网获取时间信息，更新RTC芯片的信息-->主循环。

* 子程序1：每隔10min触发，显示开机动画（辉光管防止阴性中毒程序）
* 子程序2：隔更长的一段时间触发，大概是8个小时或者工作时间亮起来（这个时间可以配置，主要作用是省电休眠）
* 子程序3、4：收到WIFI消息时、按下按键时执行任务。