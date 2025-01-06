// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "wsleds.h"

#include <pio_wsleds.pio.h>
#include <string.h>
#include <hardware/dma.h>
#include <hardware/pio.h>

#include "utils.h"
#include "defines/config.h"

static constexpr u32 c_yellow = 0b11111111'11111111'00000000;
static constexpr u32 c_green = 0b00000000'11111111'00000000;
static constexpr u32 c_red = 0b01111111'00000000'00000000;
static constexpr u32 c_white = 0b11111111'11111111'11111111;
static constexpr u32 c_blue = 0b00000000'00000000'11111111;
static constexpr u32 c_purple = 0b11111111'00000000'11111111;
static constexpr u32 c_cyan = 0b00000000'11111111'11111111;
static constexpr u32 c_off = 0b00000000'00000000'00000000;

inline u32 set_brightness(const u8 level, const u32 color) {
	// 8 is off, 7 is lowest, 0 is highest
	const u8 mask = 0b11111111 >> level;
	const u32 result = ((color & 0b11111111'00000000'00000000) >> 16 & mask) << 16 |
		((color & 0b00000000'11111111'00000000) >> 8 & mask) << 8 |
		(color & 0b00000000'00000000'11111111 & mask);
	return result;
}

static u32 buffer[MOD_WSLEDS_LED_COUNT] = { 0 };

void wsleds_init() {
	// init flag
	const u32 data[MOD_WSLEDS_LED_COUNT] = {
		c_off, c_off, c_off, c_off, c_off, c_off, c_off, c_off,
		set_brightness(7, c_yellow), set_brightness(6, c_yellow), set_brightness(5, c_yellow),
		set_brightness(4, c_yellow), set_brightness(3, c_yellow), set_brightness(2, c_yellow),
		set_brightness(1, c_yellow), set_brightness(0, c_yellow),
		set_brightness(7, c_yellow), set_brightness(6, c_yellow), set_brightness(5, c_yellow),
		set_brightness(4, c_yellow), set_brightness(3, c_yellow), set_brightness(2, c_yellow),
		set_brightness(1, c_yellow), set_brightness(0, c_yellow),
		set_brightness(3, c_green), set_brightness(2, c_green), set_brightness(1, c_green), set_brightness(0, c_green),
		set_brightness(7, c_green), set_brightness(6, c_green), set_brightness(5, c_green), set_brightness(4, c_green),
		set_brightness(3, c_green), set_brightness(2, c_green), set_brightness(1, c_green), set_brightness(0, c_green),
		set_brightness(7, c_green), set_brightness(6, c_green), set_brightness(5, c_green), set_brightness(4, c_green),
		set_brightness(0, c_red), set_brightness(7, c_red), set_brightness(6, c_red), set_brightness(5, c_red),
		set_brightness(4, c_red), set_brightness(3, c_red), set_brightness(2, c_red), set_brightness(1, c_red),
		set_brightness(0, c_red), set_brightness(7, c_red), set_brightness(6, c_red), set_brightness(5, c_red),
		set_brightness(4, c_red), set_brightness(3, c_red), set_brightness(2, c_red), set_brightness(1, c_red),
		c_off, c_off, c_off, c_off, c_off, c_off, c_off, c_off };
	memcpy(buffer, data, sizeof(data));

	// init DMA
	dma_channel_claim(MOD_WSLEDS_DMA_CH);
	dma_channel_config dma_c = dma_channel_get_default_config(MOD_WSLEDS_DMA_CH);
	channel_config_set_transfer_data_size(&dma_c, DMA_SIZE_32);
	channel_config_set_read_increment(&dma_c, true); // incr true - we loop through MOD_WSLEDS_LED_COUNT size buffer
	channel_config_set_write_increment(&dma_c, false);
	channel_config_set_dreq(&dma_c, pio_get_dreq(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, true));
	dma_channel_configure(MOD_WSLEDS_DMA_CH, &dma_c, &MOD_WSLEDS_PIO->txf[MOD_WSLEDS_SM], buffer, MOD_WSLEDS_LED_COUNT,
	                      false);
	sleep_ms(1);

	// get clock divider
	const auto clk_div = utils_calculate_pio_clk_div_ns(88);
	utils_printf("WSLEDS PIO CLK DIV: %f\n", clk_div);

	// init PIO
	const auto offset = pio_add_program(MOD_WSLEDS_PIO, &pio_wsleds_program);
	pio_sm_claim(MOD_WSLEDS_PIO, MOD_WSLEDS_SM);
	pio_wsleds_program_init(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, offset, MOD_WSLEDS_PIN, clk_div);
	pio_sm_set_enabled(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, true);
	sleep_ms(1);
}

void wsleds_test() {
	dma_channel_transfer_from_buffer_now(MOD_WSLEDS_DMA_CH, buffer, MOD_WSLEDS_LED_COUNT);
}

void wsleds_anim_flag() {
	static u32 frame = 0;
	frame++;
	if (frame % 5 == 0) {
		for (auto row = 0; row < 8; row++) {
			u32 tmp = buffer[row * 8 + 7];
			for (auto column = 7; column > 0; column--) {
				buffer[row * 8 + column] = buffer[row * 8 + column - 1];
			}
			buffer[row * 8] = tmp;
		}
		wsleds_test();
	}
}
