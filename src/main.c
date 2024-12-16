// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
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

int main() {
	stdio_init_all(); // only for serial over usb - printf

	gpio_init(25); // internal LED
	gpio_set_dir(25, GPIO_OUT);

	PIO pio = pio0; // hard claim PIO and StateMachine, if already claimed - something is wrong
	const int sm = 0;

	const uint offset = pio_add_program(pio, &numbers_program);
	pio_sm_claim(pio, sm);
	hard_assert(offset > 0);

	numbers_program_init(pio, sm, offset, MOD_NUM_DISP7, MOD_NUM_DISP6, MOD_NUM_DISP5, MOD_NUM_DISP4,
		MOD_NUM_DISP3, MOD_NUM_DISP2, MOD_NUM_DISP1, MOD_NUM_DPGROUND2, MOD_NUM_DPGROUND1, 10000.0f);
	pio_sm_set_enabled(pio, sm, true);

	while (true) {
		ping = !ping;

		const uint8_t tens = i / 10;
		const uint8_t units = i % 10;

		gpio_put(25, units == 0);

		pio_sm_put(pio, sm, numbers[tens] << 7 | numbers[units]);

		if (ping) printf("ping\n");
		else printf("pong\n");

		sleep_ms(100);

		if (i == 99) {
			i = 0;
		} else {
			i++;
		}
	}
}
