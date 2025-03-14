// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "launch.h"

#include <hardware/clocks.h>
#include <hardware/dma.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

#include "shared_config.h"
#include "state.h"
#include "utils.h"
#include "defines/config.h"
#include "shared_modules/mcp/mcp.h"

static u32 slice = 0;
static u32 channel = 0;
static i16 buffer[1] = { 0 };
static bool incrementing = true;

void launch_init() {
	gpio_set_function(MOD_LAUNCH_LED, GPIO_FUNC_PWM);

	slice = pwm_gpio_to_slice_num(MOD_LAUNCH_LED);
	channel = pwm_gpio_to_channel(MOD_LAUNCH_LED);

	mcp_cfg_set_pin_out_mode(MOD_LAUNCH_BTN, false);
	mcp_cfg_set_pull_up(MOD_LAUNCH_BTN, true);

	// init pwm
	auto pwm_c = pwm_get_default_config();
	pwm_c.top = 100;
	pwm_init(slice, &pwm_c, false);
	const auto clk_div = utils_calculate_pwm_divider(100, 10.f);
	pwm_set_clkdiv(slice, clk_div);
	utils_printf("LAUNCH PWM CLK DIV: %f\n", clk_div);
	pwm_set_phase_correct(slice, false);
	pwm_set_enabled(slice, true);
	sleep_ms(1);

	// init DMA
	if (dma_channel_is_claimed(MOD_LAUNCH_DMA_CH)) utils_error_mode(31);
	dma_channel_claim(MOD_LAUNCH_DMA_CH);
	auto dma_c = dma_channel_get_default_config(MOD_LAUNCH_DMA_CH);
	channel_config_set_transfer_data_size(&dma_c, DMA_SIZE_16);
	channel_config_set_read_increment(&dma_c, false);
	channel_config_set_write_increment(&dma_c, false);
	channel_config_set_dreq(&dma_c, DREQ_FORCE);
	dma_channel_configure(MOD_LAUNCH_DMA_CH, &dma_c, utils_pwm_cc_for_16bit(slice, channel), buffer, 1, false);
	sleep_ms(1);
}

static void anim_ready() {
	if (incrementing) {
		buffer[0] += 1;
		if (buffer[0] > 100) buffer[0] = 100;
	} else {
		buffer[0] -= 1;
		if (buffer[0] < 0) buffer[0] = 0;
	}

	dma_channel_transfer_from_buffer_now(MOD_LAUNCH_DMA_CH, buffer, 1);

	if (buffer[0] >= 100) incrementing = false;
	else if (buffer[0] <= 0) incrementing = true;
}

static void anim_error() {
	if (incrementing) {
		buffer[0] += 4;
		if (buffer[0] > 100) buffer[0] = 100;
	} else {
		buffer[0] -= 4;
		if (buffer[0] < 0) buffer[0] = 0;
	}

	dma_channel_transfer_from_buffer_now(MOD_LAUNCH_DMA_CH, buffer, 1);

	if (buffer[0] >= 100) incrementing = false;
	else if (buffer[0] <= 0) incrementing = true;
}

void launch_animation() {
	if (state.phase == PHASE_ERROR) anim_error();
	else if (state_all_ok()) anim_ready();
	else if (buffer[0] != 0) {
		incrementing = true;
		buffer[0] = 0;
		dma_channel_transfer_from_buffer_now(MOD_LAUNCH_DMA_CH, buffer, 1);
	}
}
