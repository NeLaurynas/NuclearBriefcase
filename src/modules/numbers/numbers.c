// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "numbers.h"

#include <pio_numbers.pio.h>
#include <stdio.h>
#include <hardware/dma.h>
#include <hardware/pio.h>

#include "state.h"
#include "utils.h"
#include "defines/config.h"
#include "shared_modules/mcp/mcp.h"

static constexpr u8 bits[] = {
	0b0111111, // 0
	0b0001001, // 1
	0b1011110, // 2
	0b1011011, // 3
	0b1101001, // 4
	0b1110011, // 5
	0b1110111, // 6
	0b0011001, // 7
	0b1111111, // 8
	0b1111011, // 9
	0b0000000, // 10
};

static u16 buffer[] = { 0b11111111111111 };

void numbers_init() {
	state.numbers.target = utils_random_in_range(4, 9);

	// init DMA
	if (dma_channel_is_claimed(MOD_NUM_DMA_CH)) utils_error_mode(21);
	dma_channel_claim(MOD_NUM_DMA_CH);
	dma_channel_config dma_c = dma_channel_get_default_config(MOD_NUM_DMA_CH);
	channel_config_set_transfer_data_size(&dma_c, DMA_SIZE_16);
	channel_config_set_read_increment(&dma_c, false);
	// incr false - we always read from same memory location (array of size 1)
	channel_config_set_write_increment(&dma_c, false);
	channel_config_set_dreq(&dma_c, DREQ_FORCE); // immediately push no matter what
	dma_channel_configure(MOD_NUM_DMA_CH, &dma_c, &MOD_NUM_PIO->txf[MOD_NUM_SM], buffer, 1, false);
	sleep_ms(1);

	// get clock divider
	const auto clk_div = utils_calculate_pio_clk_div(175);
	utils_printf("NUMBERS PIO CLK DIV: %f\n", clk_div);

	// init PIO
	const auto offset = pio_add_program(MOD_NUM_PIO, &pio_numbers_program);
	if (offset < 0) utils_error_mode(22);
	if (pio_sm_is_claimed(MOD_NUM_PIO, MOD_NUM_SM)) utils_error_mode(23);
	pio_sm_claim(MOD_NUM_PIO, MOD_NUM_SM);
	pio_numbers_program_init(MOD_NUM_PIO, MOD_NUM_SM, offset, MOD_NUM_DISP7, MOD_NUM_DISP6, MOD_NUM_DISP5,
	                         MOD_NUM_DISP4, MOD_NUM_DISP3, MOD_NUM_DISP2, MOD_NUM_DISP1, MOD_NUM_DPGROUND2,
	                         MOD_NUM_DPGROUND1, clk_div);
	pio_sm_set_enabled(MOD_NUM_PIO, MOD_NUM_SM, true);
	sleep_ms(1);

	// init MCP
	mcp_cfg_set_pin_out_mode(MOD_NUM_LED_G, true);
	mcp_cfg_set_pin_out_mode(MOD_NUM_LED_R, true);
	mcp_cfg_set_pin_out_mode(MOD_NUM_ENC1, false);
	mcp_cfg_set_pull_up(MOD_NUM_ENC1, true);
	mcp_cfg_set_pin_out_mode(MOD_NUM_ENC2, false);
	mcp_cfg_set_pull_up(MOD_NUM_ENC2, true);
	mcp_cfg_set_pin_out_mode(MOD_NUM_BTN, false);
	mcp_cfg_set_pull_up(MOD_NUM_BTN, true);
}

void numbers_display(const u8 number1, const u8 number2) {
	buffer[0] = bits[number1] << 7 | bits[number2];
	dma_channel_transfer_from_buffer_now(MOD_NUM_DMA_CH, buffer, 1);
}

void numbers_generate_target() {
	u8 target = utils_random_in_range(0, 9);

	while (target == state.numbers.number || target == state.numbers.target) {
		target = (target + 1) % 10;
	}
	state.numbers.target = target;
}

void numbers_inc() {
	state.numbers.number = (state.numbers.number + 1) % 10;
}

void numbers_dec() {
	if (state.numbers.number == 0) state.numbers.number = 9;
	else state.numbers.number -= 1;
}
