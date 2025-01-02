// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef CONFIG_H
#define CONFIG_H

#include <hardware/i2c.h>

#define DBG true

#define RENDER_TICK 10'000

#define MOD_NUM_DPGROUND1 15
#define MOD_NUM_DPGROUND2 14
#define MOD_NUM_DISP1 13 // module numbers display pin 1
#define MOD_NUM_DISP2 12
#define MOD_NUM_DISP3 11
#define MOD_NUM_DISP4 10
#define MOD_NUM_DISP5 9
#define MOD_NUM_DISP6 8
#define MOD_NUM_DISP7 7
#define MOD_NUM_PIO pio0
#define MOD_NUM_SM 0
#define MOD_NUM_DMA_CH 0
/* MCP Layout (one byte):
 *   bit 7: MCP 1 or MCP 2
 *   bit 6: Bank A or Bank B
 *   rest: 6-bit number (max val 63)
 *    7   6   5   4   3   2   1   0
 *  | M | B |       number          |
 */
#define MOD_NUM_LEDR	(uint8_t)((0 << 7) | (1 << 6) | 0) // MCP - first bit - MCP 1, second bit - Bank B - pin 0
#define MOD_NUM_LEDG	(uint8_t)((0 << 7) | (1 << 6) | 1) // MCP - first bit - MCP 1, second bit - Bank B - pin 1
#define MOD_NUM_BTN		(uint8_t)((0 << 7) | (1 << 6) | 2) // MCP - first bit - MCP 1, second bit - Bank B - pin 2
#define MOD_NUM_ENC1	(uint8_t)((0 << 7) | (1 << 6) | 3)
#define MOD_NUM_ENC2	(uint8_t)((0 << 7) | (1 << 6) | 4)
#define MOD_NUM_ENC_DEBOUNCE_MS 66

#define MOD_STAT_LED_NUMBERS (uint8_t)((1 << 7) | (1 << 6) | 0)

#define MOD_MCP_PIN_SDA 16
#define MOD_MCP_PIN_SCL 17
#define MOD_MCP_I2C_PORT (&i2c0_inst)
#define MOD_MCP_ADDR1 0x20
#define MOD_MCP_ADDR2 0x21
#define MOD_MCP_GPIO_CACHE_MS 6

#endif //CONFIG_H
