// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <hardware/dma.h>

#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

#include "numbers.pio.h"
#include "defines/config.h"

bool ping = true;
uint8_t i = 0;

uint8_t numbers[] = {
	0b1110111, // 0
	0b0010001, // 1
	0b1101011, // 2
	0b0111011, // 3
	0b0011101, // 4
	0b0111110, // 5
	0b1111110, // 6
	0b0010011, // 7
	0b1111111, // 8
	0b0111111, // 9
};

uint16_t numbers_buffer[] = {0b11111111111111};

float read_onboard_temperature() {

	/* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
	const float conversionFactor = 3.3f / (1 << 12);

	float adc = (float)adc_read() * conversionFactor;
	float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

	return tempC;
}

int main() {
	stdio_init_all(); // only for serial over usb - printf

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	gpio_init(25); // internal LED
	gpio_set_dir(25, GPIO_OUT);

	PIO pio = pio0; // hard claim PIO, DMA and StateMachine, if already claimed - something is wrong
	const int sm = 0;
	const int dma_cha = dma_claim_unused_channel(true);

	const uint offset = pio_add_program(pio, &numbers_program);
	pio_sm_claim(pio, sm);
	hard_assert(offset > 0);

	numbers_program_init(pio, sm, offset, MOD_NUM_DISP7, MOD_NUM_DISP6, MOD_NUM_DISP5, MOD_NUM_DISP4,
	                     MOD_NUM_DISP3, MOD_NUM_DISP2, MOD_NUM_DISP1, MOD_NUM_DPGROUND2, MOD_NUM_DPGROUND1, 10000.0f);
	pio_sm_set_enabled(pio, sm, true);

	dma_channel_config dma_c = dma_channel_get_default_config(dma_cha);
	channel_config_set_transfer_data_size(&dma_c, DMA_SIZE_16);
	channel_config_set_read_increment(&dma_c, false); // why true? True to enable read address increments, if false, each read will be from the same address Usually disabled for peripheral to memory transfers
	channel_config_set_write_increment(&dma_c, false); // why false? if false, each write will be to the same address - statemachine address will not change
	channel_config_set_dreq(&dma_c, DREQ_FORCE);

	dma_channel_configure(dma_cha, &dma_c, &pio->txf[sm], numbers_buffer, 1, false);

	while (true) {
		ping = !ping;
		sleep_ms(100);

		const uint8_t tens = i / 10;
		const uint8_t units = i % 10;

		gpio_put(25, units == 0);

		// pio_sm_put(pio, sm, numbers[tens] << 7 | numbers[units]); // can use _blocking, but we'll never saturate FIFO buffer
		numbers_buffer[0] = numbers[tens] << 7 | numbers[units];
		dma_channel_transfer_from_buffer_now(dma_cha, &numbers_buffer, 1);

		// if (ping) printf("ping\n");
		// else printf("pong\n");



		if (units == 0 && tens % 2 == 0) {
			const float temperature = read_onboard_temperature();
			printf("Onboard temperature = %.02f C\n", temperature);
		}

		if (i == 99) { i = 0; }
		else { i++; }
	}
}
