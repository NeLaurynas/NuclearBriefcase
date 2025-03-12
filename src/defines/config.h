// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef CONFIG_H
#define CONFIG_H

#include <hardware/i2c.h>
#include <pico/types.h>

#define RENDER_TICK 10'000
#define ANIM_FRAME_COUNT 10'000

#define MOD_NUM_DPGROUND1	15
#define MOD_NUM_DPGROUND2	14
#define MOD_NUM_DISP1		13 // module numbers display pin 1
#define MOD_NUM_DISP2		12
#define MOD_NUM_DISP3		11
#define MOD_NUM_DISP4		10
#define MOD_NUM_DISP5		9
#define MOD_NUM_DISP6		8
#define MOD_NUM_DISP7		7
#define MOD_NUM_PIO			pio0
#define MOD_NUM_SM			0
#define MOD_NUM_DMA_CH		0
/* MCP Layout (one byte):
 *   bit 7: MCP 1 or MCP 2
 *   bit 6: Bank A or Bank B
 *   rest: 6-bit number (max val 63)
 *    7   6   5   4   3   2   1   0
 *  | M | B |       number          |
 */
#define MOD_NUM_LEDR	(u8)((1 << 7) | (1 << 6) | 0) // MCP - first bit - MCP 2, second bit - Bank B - pin 0
#define MOD_NUM_LEDG	(u8)((1 << 7) | (1 << 6) | 1) // MCP - first bit - MCP 2, second bit - Bank B - pin 1
#define MOD_NUM_BTN		(u8)((1 << 7) | (1 << 6) | 2) // MCP - first bit - MCP 2, second bit - Bank B - pin 2
#define MOD_NUM_ENC1	(u8)((1 << 7) | (1 << 6) | 3)
#define MOD_NUM_ENC2	(u8)((1 << 7) | (1 << 6) | 4)
#define MOD_NUM_ENC_DEBOUNCE_MS 66

#define MOD_STAT_LED_NUMBERS (u8)((1 << 7) | (1 << 6) | 0)

#define MOD_WSLEDS_LED_COUNT	64
#define MOD_WSLEDS_PIO			pio0
#define MOD_WSLEDS_SM			1
#define MOD_WSLEDS_DMA_CH		1
#define MOD_WSLEDS_PIN			18

#define MOD_PIEZO_PIN			19
#define MOD_PIEZO_DMA_CH_TOP	2
#define MOD_PIEZO_DMA_CH_CC		3

#define MOD_LAUNCH_BTN		0
#define MOD_LAUNCH_LED		20
#define MOD_LAUNCH_DMA_CH	4

#define MOD_SWITCHES_SWITCH1	(u8)((0 << 7) | (0 << 6) | 0) // MCP 1 | Bank A | PIN 0
#define MOD_SWITCHES_SWITCH2	(u8)((0 << 7) | (0 << 6) | 1) // MCP 1 | Bank A | PIN 1
#define MOD_SWITCHES_LED1_R		(u8)((1 << 7) | (0 << 6) | 0) // MCP 2 | Bank A | PIN 0
#define MOD_SWITCHES_LED1_G		(u8)((1 << 7) | (0 << 6) | 1) // MCP 2 | Bank A | PIN 1
#define MOD_SWITCHES_LED2_R		(u8)((1 << 7) | (0 << 6) | 2) // MCP 2 | Bank A | PIN 2
#define MOD_SWITCHES_LED2_G		(u8)((1 << 7) | (0 << 6) | 3) // MCP 2 | Bank A | PIN 3

#endif //CONFIG_H
