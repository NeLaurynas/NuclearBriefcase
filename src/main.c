// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <stdio.h>
#include <hardware/i2c.h>

#include "state.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "modules/mcp/mcp.h"
#include "modules/numbers/numbers.h"
#include "pico/stdlib.h"

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
	set_sys_clock_khz(48000, false);

	stdio_init_all(); // only for serial over usb - printf

	numbers_init();
	mcp_init();

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	gpio_init(25); // internal LED
	gpio_set_dir(25, GPIO_OUT);

	while (true) {
		ping = !ping;
		sleep_ms(500);

		// printf("Scanning...\n");
		// for (uint8_t addr = 0; addr < 127; addr++) {
		// 	if (i2c_write_blocking(MOD_MCP_I2C_PORT, addr, NULL, 0, false) >= 0) {
		// 		printf("Device found at 0x%02X\n", addr);
		// 	}
		// }
		// sleep_ms(1000);

		const uint8_t tens = i / 10;
		const uint8_t units = i % 10;

		gpio_put(25, units == 0);

		numbers_display(tens, units);

		mcp_all();

		if (units == 0 && tens % 2 == 0) {
			const float temperature = read_onboard_temperature();
			printf("Onboard temperature = %.02f C\n", temperature);
			uint32_t sys_clk_khz = clock_get_hz(clk_sys) / 1000;
			// printf("System clock: %u kHz\n", sys_clk_khz);
		}

		if (i == 99) { i = 0; }
		else { i++; }
	}
}
