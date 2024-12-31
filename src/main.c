// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include <string.h>
#include <hardware/i2c.h>

#include "renderer.h"
#include "state.h"
#include "utils.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "modules/mcp/mcp.h"
#include "modules/numbers/numbers.h"
#include "pico/stdlib.h"

int main() {
	set_sys_clock_khz(48'000, false);

	stdio_init_all(); // only for serial over usb - printf

	numbers_init();
	mcp_init();

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	gpio_init(25); // internal LED
	gpio_set_dir(25, GPIO_OUT);

	memcpy(&currentState, &state, sizeof(State));

	renderer_loop();
}
