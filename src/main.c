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

#include "blink.pio.h"

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

int main() {
	// only for usb printf
	stdio_init_all();

	// on board led
	gpio_init(25);
	gpio_set_dir(25, GPIO_OUT);

	// TODO: picotool load -f blink_simple.uf2 after build

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

	while (true) {
		gpio_put(25, on);
		on = !on;

		printf("Toggle LED\n");
		sleep_ms(1000);
	}
}
