// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>
#include <hardware/i2c.h>

#include "renderer.h"
#include "utils.h"
#include "defines/config.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "modules/mcp/mcp.h"
#include "modules/numbers/numbers.h"
#include "pico/stdlib.h"

int main() {
	set_sys_clock_khz(48'000, false);

	stdio_init_all(); // only for serial over usb - printf

#if DBG
	sleep_ms(2000);
	utils_printf("Slept for 2 seconds\n");
#endif

	mcp_init(); // init first, other modules use mcp...
	numbers_init();

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	gpio_init(25); // internal LED
	gpio_set_dir(25, GPIO_OUT);

	renderer_loop();
}
