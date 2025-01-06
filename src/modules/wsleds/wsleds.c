// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "wsleds.h"

#include <pio_wsleds.pio.h>
#include <hardware/dma.h>
#include <hardware/pio.h>

#include "utils.h"
#include "defines/config.h"

static constexpr u32 c_yellow = 0b11111111'11111111'00000000;
static constexpr u32 c_green = 0b00000000'11111111'00000000;
static constexpr u32 c_red = 0b11111111'00000000'00000000;
static constexpr u32 c_white = 0b11111111'11111111'11111111;
static constexpr u32 c_blue = 0b00000000'00000000'11111111;
static constexpr uint32_t c_purple = 0b11111111'00000000'11111111;
static constexpr uint32_t c_off = 0b00000000'00000000'00000000;
static u32 buffer[MOD_WSLEDS_LED_COUNT] = {
	c_off, c_off, c_off, c_off, c_off, c_off, c_off, c_off,
	/*8*/ c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, c_yellow,
	c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, /*23*/
	/*24*/c_green, c_green, c_green, c_green, c_green, c_green, c_green, c_green,
	c_green, c_green, c_green, c_green, c_green, c_green, c_green, c_green, /*39*/
	/*40*/c_red, c_red, c_red, c_red, c_red, c_red, c_red, c_red,
	c_red, c_red, c_red, c_red, c_red, c_red, c_red, c_red, /*55*/
	c_off, c_off, c_off, c_off, c_off, c_off, c_off, c_off
};

void wsleds_init() {
	// init DMA
	// dma_channel_claim(MOD_WSLEDS_DMA_CH);
	// dma_channel_config dma_c = dma_channel_get_default_config(MOD_WSLEDS_DMA_CH);
	// channel_config_set_transfer_data_size(&dma_c, DMA_SIZE_32);
	// channel_config_set_read_increment(&dma_c, true); // incr true - we loop through MOD_WSLEDS_LED_COUNT size buffer
	// channel_config_set_write_increment(&dma_c, false);
	// channel_config_set_dreq(&dma_c, DREQ_FORCE);
	// dma_channel_configure(MOD_WSLEDS_DMA_CH, &dma_c, &MOD_WSLEDS_PIO->txf[MOD_WSLEDS_SM], buffer, MOD_WSLEDS_LED_COUNT,
	//                       false);
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

inline u32 set_brightness(const u8 level, const u32 color) {
	// 8 is off, 7 is lowest, 0 is highest
    const u8 mask = 0b11111111 >> level;
    const u32 result = ((color & 0b11111111'00000000'00000000) >> 16 & mask) << 16 |
                      ((color & 0b00000000'11111111'00000000) >> 8  & mask) << 8  |
                      (color & 0b00000000'00000000'11111111 & mask);
    return result;
}

void wsleds_test() {
	// dma_channel_transfer_from_buffer_now(MOD_WSLEDS_DMA_CH, buffer, MOD_WSLEDS_LED_COUNT);
	static bool is_up = true;
	static i8 level = 9; // initial, will get decreased to 8


	if (is_up) level -= 1; // from 7 to 6
	else level += 1; // from 0 to 1

	// set next data
	if (level == -1) {
		is_up = false;
		level = 1;
	} else if (level == 9) {
		is_up = true;
		level = 7;
	}

	for (auto i = 0; i < 64; i++) { // put data
		pio_sm_put_blocking(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, buffer[i]);
	}
	// set yellow
	for (auto i = 8; i < 24; i++) {
		buffer[i] = set_brightness(level, c_yellow);
	}
	// set green
	for (auto i = 24; i < 40; i++) {
		buffer[i] = set_brightness(level, c_green);
	}
	// set red
	for (auto i = 40; i < 56; i++) {
		buffer[i] = set_brightness(level, c_red);
	}
}
