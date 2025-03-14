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
#include "modules/launch/launch.h"
#include "modules/numbers/numbers.h"
#include "modules/piezo/piezo.h"
#include "modules/status/status.h"
#include "modules/switches/switches.h"
#include "modules/wsleds/wsleds.h"
#include "pico/stdlib.h"
#include "shared_modules/mcp/mcp.h"

int main() {
	gpio_init(INTERNAL_LED);
	gpio_set_dir(INTERNAL_LED, GPIO_OUT);

	const void (*animation_functions[])() = { piezo_animation, wsleds_animation, launch_animation, switches_animation };
	constexpr u8 anim_fn_size = ARRAY_SIZE(animation_functions);

	if (!set_sys_clock_khz(48'000, false)) utils_error_mode(47); // minimum to enable USB
	// if (!set_sys_clock_khz(18'000, false)) utils_error_mode(47);

	stdio_init_all(); // only for serial over usb/uart - printf (also uploading..)

	wsleds_init();
	mcp_init();
	numbers_init();
	status_init();
	piezo_init();
	switches_init();
	launch_init();

	state_set_minus();

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	renderer_init(animation_functions, anim_fn_size);

	renderer_loop();
}
