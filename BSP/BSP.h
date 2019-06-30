#pragma once

// Debug Port
#define Debug_UARTHANDLE            huart2

// Button
// PH3
#define Button_GPIO_PORT            GPIOH
#define Button_PIN                  GPIO_PIN_3
#define Button_IRQ                  EXTI3_IRQn

// HV507
// CLK DATA-->SPI1(Transmit Only Master Mode)
// LE-->PB4
#define HV507_LE_GPIO_PORT          GPIOB
#define HV507_LE_GPIO_PIN           GPIO_PIN_4
#define HV507_SPI                   SPI1
#define HV507_SPIHANDLE             hspi1

// DS3231
// SCL SCA-->I2C1
// SQW/INT-->P?13
#define DS3231_I2C                  I2C1
#define DS3231_I2CHANDLE            hi2c1
#define DS3231_SQW_INT_GPIO_PORT    GPIOC
#define DS3231_SQW_INT_GPIO_PIN     GPIO_PIN_13
#define DS3231_SQW_INT_IRQ          EXTI15_10_IRQn 

// WS2812
// PA15
#define WS2812_GPIO_PORT            GPIOA
#define WS2812_GPIO_PIN             GPIO_PIN_15

// PWR
// V5_EN-->PA5
// VPP_EN-->PA4
#define V5_GPIO_PORT                GPIOA
#define V5_GPIO_PIN                 GPIO_PIN_5
#define VPP_GPIO_PORT               GPIOA
#define VPP_GPIO_PIN                GPIO_PIN_4

// ESP8266 Wifi
// RST-->PA8
#define WIFI_UART                   USART1
#define WIFI_UARTHANDLE             huart1
#define WIFI_RST_GPIO_PORT          GPIOA
#define WIFI_RST_GPIO_PIN           GPIO_PIN_8

// W25QXX
// QSPI FLASH
#define W25QXX_QSPIHANDLE           hqspi