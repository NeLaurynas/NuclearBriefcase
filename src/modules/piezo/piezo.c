// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "piezo.h"

#include <hardware/gpio.h>
#include <hardware/pwm.h>

#include "utils.h"
#include "defines/config.h"

void piezo_init() {
	gpio_set_function(MOD_PIEZO_PIN, GPIO_FUNC_PWM);

	uint slice = pwm_gpio_to_slice_num(MOD_PIEZO_PIN);
	uint channel = pwm_gpio_to_channel(MOD_PIEZO_PIN);

	auto config = pwm_get_default_config();

	const auto clk_div = utils_calculate_pio_clk_div(10);
	utils_printf("PIEZO PWM CLK DIV: %f\n", clk_div);

	pwm_init(slice, &config, false);

	pwm_set_wrap(slice, 99); // [0, 99] - 100 cycles
	for (auto i = 0; i < 50; i++) {
		pwm_set_chan_level(slice, channel, 1);
	}
	for (auto i = 50; i < 100; i++) {
		pwm_set_chan_level(slice, channel, 0);
	}

	pwm_set_enabled(slice, true);

	// init DMA
}
