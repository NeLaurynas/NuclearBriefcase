// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef CONFIG_H
#define CONFIG_H

#include <hardware/i2c.h>

#define MOD_NUM_DPGROUND1 15
#define MOD_NUM_DPGROUND2 14
#define MOD_NUM_DISP1 13 // module numbers display pin 1
#define MOD_NUM_DISP2 12
#define MOD_NUM_DISP3 11
#define MOD_NUM_DISP4 10
#define MOD_NUM_DISP5 9
#define MOD_NUM_DISP6 8
#define MOD_NUM_DISP7 7
#define MOD_NUM_BTN 0
#define MOD_NUM_PIO pio0
#define MOD_NUM_SM 0
#define MOD_NUM_DMA_CH 0

#define MOD_MCP_PIN_SDA 16
#define MOD_MCP_PIN_SCL 17
#define MOD_MCP_I2C_PORT (&i2c0_inst)
#define MOD_MCP_ADDR_1 0x20


#endif //CONFIG_H
