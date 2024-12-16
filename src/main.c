// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA 8
#define I2C_SCL 9

#include "numbers.pio.h"
#include "defines/config.h"
#include "defines/gp_masks.h"

void blink_pin_forever(PIO pio, uint sm, uint offset, uint pin, uint freq) {
	// blink_program_init(pio, sm, offset, pin);
	// pio_sm_set_enabled(pio, sm, true);
	//
	// printf("Blinking pin %d at %d Hz\n", pin, freq);
	//
	// // PIO counter program takes 3 more cycles in total than we pass as
	// // input (wait for n + 1; mov; jmp)
	// pio->txf[sm] = 125000000 / (2 * freq) - 3;
}

bool on = true;

uint MASK_ALL = GPM_0 | GPM_1 | GPM_2 | GPM_3 | GPM_4 |
					GPM_5 | GPM_6 | GPM_7 | GPM_8 | GPM_9 |
					GPM_10 | GPM_11 | GPM_12 | GPM_13 | GPM_14 |
					GPM_15 | GPM_16 | GPM_17 | GPM_18 | GPM_19 |
					GPM_20 | GPM_21 | GPM_22 |
					GPM_INTERNAL_LED | GPM_26 | GPM_27 | GPM_28;

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
	// only for usb printf
	stdio_init_all();

	// ALL TEST
	// gpio_init_mask(MASK_ALL);
	// gpio_set_dir_out_masked(MASK_ALL);
	// gpio_put_all(MASK_ALL);
	// gpio_put_masked(MASK_ALL, 1); // value is also a mask...

	gpio_init(25);
	gpio_set_dir(25, GPIO_OUT);

	// gpio_init(MOD_NUM_DPGROUND2);
	// gpio_set_dir(MOD_NUM_DPGROUND2, GPIO_OUT);
	// gpio_init(MOD_NUM_DPGROUND1);
	// gpio_set_dir(MOD_NUM_DPGROUND1, GPIO_OUT);
	// gpio_init(MOD_NUM_DISP1);
	// gpio_set_dir(MOD_NUM_DISP1, GPIO_OUT);
	// gpio_init(MOD_NUM_DISP2);
	// gpio_set_dir(MOD_NUM_DISP2, GPIO_OUT);
	// gpio_init(MOD_NUM_DISP3);
	// gpio_set_dir(MOD_NUM_DISP3, GPIO_OUT);
	// gpio_init(MOD_NUM_DISP4);
	// gpio_set_dir(MOD_NUM_DISP4, GPIO_OUT);
	// gpio_init(MOD_NUM_DISP5);
	// gpio_set_dir(MOD_NUM_DISP5, GPIO_OUT);
	// gpio_init(MOD_NUM_DISP6);
	// gpio_set_dir(MOD_NUM_DISP6, GPIO_OUT);
	// gpio_init(MOD_NUM_DISP7);
	// gpio_set_dir(MOD_NUM_DISP7, GPIO_OUT);

	// all pins test
	// gpio_init_mask()

	// I2C Initialisation. Using it at 400Khz.
	// i2c_init(I2C_PORT, 400 * 1000);
	//
	// gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
	// gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
	// gpio_pull_up(I2C_SDA);
	// gpio_pull_up(I2C_SCL);
	// For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

	// PIO Blinking example
	// PIO pio = pio0;
	// uint offset = pio_add_program(pio, &blink_program);
	// printf("Loaded program at %d\n", offset);

	// #ifdef PICO_DEFAULT_LED_PIN
	//     blink_pin_forever(pio, 0, offset, PICO_DEFAULT_LED_PIN, 3);
	// #else
	//     blink_pin_forever(pio, 0, offset, 6, 3);
	// #endif
	//     // For more pio examples see https://github.com/raspberrypi/pico-examples/tree/master/pio

	PIO pio = pio0;
	int sm = 0;
	uint offset = pio_add_program(pio, &numbers_program);
	printf("Loaded program at %d\n", offset);
	pio_sm_claim(pio, sm);
	numbers_program_init(pio, sm, offset, MOD_NUM_DISP7, MOD_NUM_DISP6, MOD_NUM_DISP5, MOD_NUM_DISP4,
		MOD_NUM_DISP3, MOD_NUM_DISP2, MOD_NUM_DISP1, MOD_NUM_DPGROUND2, MOD_NUM_DPGROUND1,
		10000.0);
	pio_sm_set_enabled(pio, sm, true);

	uint i = 0;

	while (true) {
		// gpio_put(25, on);
		// gpio_put(MOD_NUM_DPGROUND1, on);
		// gpio_put(MOD_NUM_DPGROUND2, !on);
		// gpio_put(MOD_NUM_DISP1, 1);
		// gpio_put(MOD_NUM_DISP2, 1);
		// gpio_put(MOD_NUM_DISP3, 1);
		// gpio_put(MOD_NUM_DISP4, 1);
		// gpio_put(MOD_NUM_DISP5, 1);
		// gpio_put(MOD_NUM_DISP6, 1);
		// gpio_put(MOD_NUM_DISP7, 1);
		on = !on;

		int tens = i / 10;
		int units = i % 10;

		gpio_put(25, units == 0);

		pio_sm_put_blocking(pio, sm, numbers[tens] << 7 | numbers[units]);

		if (on) {
			// pio_sm_put_blocking(pio, sm, 0b11111100111111); // 7 bytes - first number, 7 bytes - second number
		} else {
			// pio_sm_put_blocking(pio, sm, 0b00000001111111);
		}
		// 0b00000000000000
		// 0b11111111111111

		if (on) printf("ping\n");
		else printf("pong\n");
		sleep_ms(100);

		if (i == 99) {
			i = 0;
		} else {
			i++;
		}
	}
}
