// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <pio_numbers.pio.h>
#include <hardware/dma.h>
#include <hardware/pio.h>

#include "numbers.h"
#include "defines/config.h"

uint8_t bytes[] = {
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

// blank line circle
// uint8_t animation_bytes[] = {
// 	0b0110111, // 0
// 	0b1010111, // 1
// 	0b1100111, // 2
// 	0b1110110, // 3
// 	0b1110101, // 4
// 	0b1110011, // 5
// };
uint8_t animation_bytes[] = {
	0b1000000, // 0
	0b0100000, // 1
	0b0010000, // 2
	0b0000001, // 3
	0b0000010, // 4
	0b0000100, // 5
};

uint16_t buffer[] = {0b11111111111111};

void numbers_init() {
	// init DMA
	dma_channel_claim(MOD_NUM_DMA_CH);
	dma_channel_config dma_c = dma_channel_get_default_config(MOD_NUM_DMA_CH);
	channel_config_set_transfer_data_size(&dma_c, DMA_SIZE_16);
	channel_config_set_read_increment(&dma_c, false); // incr false - we always read from same memory location
	channel_config_set_write_increment(&dma_c, false);
	channel_config_set_dreq(&dma_c, DREQ_FORCE);
	dma_channel_configure(MOD_NUM_DMA_CH, &dma_c, &MOD_NUM_PIO->txf[MOD_NUM_SM], buffer, 1, false);

	// init PIO
	const uint offset = pio_add_program(MOD_NUM_PIO, &pio_numbers_program);
	hard_assert(offset > 0); // TODO: led blinking error module
	pio_sm_claim(MOD_NUM_PIO, MOD_NUM_SM);
	pio_numbers_program_init(MOD_NUM_PIO, MOD_NUM_SM, offset, MOD_NUM_DISP7, MOD_NUM_DISP6, MOD_NUM_DISP5,
	                         MOD_NUM_DISP4, MOD_NUM_DISP3, MOD_NUM_DISP2, MOD_NUM_DISP1, MOD_NUM_DPGROUND2,
	                         MOD_NUM_DPGROUND1, 333.3f);
	pio_sm_set_enabled(MOD_NUM_PIO, MOD_NUM_SM, true);
}

void numbers_display(uint8_t number1, uint8_t number2) {
	buffer[0] = bytes[number1] << 7 | bytes[number2];
	dma_channel_transfer_from_buffer_now(MOD_NUM_DMA_CH, &buffer, 1);
}

void anim(uint8_t frame) {
	buffer[0] = animation_bytes[frame] << 7 | animation_bytes[frame];
	dma_channel_transfer_from_buffer_now(MOD_NUM_DMA_CH, &buffer, 1);
}
