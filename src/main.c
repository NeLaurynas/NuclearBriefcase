// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>

#include "hardware/adc.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "pico/stdlib.h"

#include "modules/numbers/numbers.h"

bool ping = true;
uint8_t i = 0;

float read_onboard_temperature() {

	/* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
	const float conversionFactor = 3.3f / (1 << 12);

	float adc = (float)adc_read() * conversionFactor;
	float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

	return tempC;
}

int main() {
	stdio_init_all(); // only for serial over usb - printf
	numbers_init();

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	gpio_init(25); // internal LED
	gpio_set_dir(25, GPIO_OUT);

	while (true) {
		ping = !ping;
		sleep_ms(100);

		const uint8_t tens = i / 10;
		const uint8_t units = i % 10;

		gpio_put(25, units == 0);

		numbers_display_both(tens, units);
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
