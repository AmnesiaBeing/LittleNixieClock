#pragma once

#include <stdint.h>
#include <stdbool.h>

// 按照W25Q128JV数据手册上的指令顺序写的

#define W25QXX_QSPIHANDLE                       hqspi

/* Write Operations */
#define WRITE_ENABLE_CMD                        0x06
#define VOL_SR_WRITE_ENABLE_CMD                 0x50
#define WRITE_DISABLE_CMD                       0x04

/* Identification Operations */
#define RELEASE_PWRDOWN_CMD                     0xAB
#define READ_DEVICE_ID_CMD                      0x90
#define READ_JEDEC_ID_CMD                       0x9F
#define READ_UNIQUE_ID_CMD                      0x4B

/* Read Operations */
#define READ_CMD                                0x03
#define FAST_READ_CMD                           0x0B
#define DUAL_OUT_FAST_READ_CMD                  0x3B
#define DUAL_INOUT_FAST_READ_CMD                0xBB
#define QUAD_OUT_FAST_READ_CMD                  0x6B
#define QUAD_INOUT_FAST_READ_CMD                0xEB

/* Program Operations */
#define PAGE_PROG_CMD                           0x02
#define DUAL_IN_FAST_PROG_CMD                   0xA2
#define EXT_DUAL_IN_FAST_PROG_CMD               0xD2
#define QUAD_IN_FAST_PROG_CMD                   0x32
#define EXT_QUAD_IN_FAST_PROG_CMD               0x12

/* Register Operations */
#define READ_STATUS_REG1_CMD                    0x05
#define WRITE_STATUS_REG1_CMD                   0x01
#define READ_STATUS_REG2_CMD                    0x35
#define WRITE_STATUS_REG2_CMD                   0x31
#define READ_STATUS_REG3_CMD                    0x15
#define WRITE_STATUS_REG3_CMD                   0x11

/* ? */
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD       0x5A
#define ERASE_SR_CMD                            0x44
#define PROG_SR_CMD                             0x42
#define READ_SR_CMD                             0x48

/* ? */
#define GLOBAL_LOCK_CMD                         0x7E
#define GLOBAL_UNLOCK_CMD                       0x98
#define READ_BLOCK_LOCK_CMD                     0x3D
#define WRITE_BLOCK_LOCK_CMD                    0xE5

/* Erase Operations */
#define PROG_ERASE_SUSPEND_CMD                  0x75
#define PROG_ERASE_RESUME_CMD                   0x7A

/* ? */
#define PWR_DOWN_CMD                            0xB9

/* Reset Operations */
#define RESET_ENABLE_CMD                        0x66
#define RESET_MEMORY_CMD                        0x99
//####################################################
#define W25Q128_ID                              0xEF4018


void W25QXX_Init(void);
void W25QXX_Reset(void);
uint32_t W25QXX_ReadID(void);
void W25QXX_Enable_QSPIMODE(void);
void W25QXX_WriteEnable(void);