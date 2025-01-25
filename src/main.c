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
#include "modules/numbers/numbers.h"
#include "modules/piezo/piezo.h"
#include "modules/status/status.h"
#include "modules/wsleds/wsleds.h"
#include "pico/stdlib.h"
#include "data/firetruck_siren.h"

int main() {
	gpio_init(INTERNAL_LED);
	gpio_set_dir(INTERNAL_LED, GPIO_OUT);

	const void (*animation_functions[])() = { wsleds_animation, piezo_animation };
	constexpr u8 anim_fn_size = ARRAY_SIZE(animation_functions);

	// if (!set_sys_clock_khz(48'000, false)) utils_error_mode(47); // minimum to enable USB
	if (!set_sys_clock_khz(18'000, false)) utils_error_mode(47);

#if DBG
	stdio_init_all(); // only for serial over usb/uart - printf
#endif

#if DBG
	sleep_ms(2000);
	utils_printf("slept for 2 seconds\n");
#endif

	utils_printf("!!!!!!!!!!!!!! %d\n", firetruck_siren_mp3[0]);

	wsleds_init();
	// mcp_init();
	// numbers_init();
	// status_init();
	piezo_init();

	renderer_init(animation_functions, anim_fn_size);

	adc_init();
	adc_set_temp_sensor_enabled(true);
	adc_select_input(4);

	renderer_loop();
}
