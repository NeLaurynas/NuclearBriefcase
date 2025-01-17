// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "piezo.h"

#include <hardware/clocks.h>
#include <hardware/dma.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

#include "state.h"
#include "utils.h"
#include "defines/config.h"

static uint slice = 0;
static uint channel = 0;
static u32 clock_freq_hz = 0;
static u32 buffer_top[0] = { 0 };
static u32 buffer_cc[0] = { 0 };

void piezo_init() {
	gpio_set_function(MOD_PIEZO_PIN, GPIO_FUNC_PWM);

	slice = pwm_gpio_to_slice_num(MOD_PIEZO_PIN);
	channel = pwm_gpio_to_channel(MOD_PIEZO_PIN);
	// 28

	auto pwm_c = pwm_get_default_config();
	pwm_c.top = 0;
	pwm_init(slice, &pwm_c, false);
	pwm_set_clkdiv(slice, 75.f);
	pwm_set_phase_correct(slice, false); // true - counts to wrap for up, then counts down from top to wrap, for precision
	pwm_set_enabled(slice, true);
	sleep_ms(1);

	// init DMA
	// init top
	if (dma_channel_is_claimed(MOD_PIEZO_DMA_CH_TOP)) utils_error_mode(28);
	dma_channel_claim(MOD_PIEZO_DMA_CH_TOP);
	dma_channel_config dma_top_c = dma_channel_get_default_config(MOD_PIEZO_DMA_CH_TOP);
	channel_config_set_transfer_data_size(&dma_top_c, DMA_SIZE_32);
	channel_config_set_read_increment(&dma_top_c, false);
	channel_config_set_write_increment(&dma_top_c, false);
	channel_config_set_dreq(&dma_top_c, DREQ_FORCE);
	dma_channel_configure(MOD_PIEZO_DMA_CH_TOP, &dma_top_c, &pwm_hw->slice[slice].top, buffer_top, 1, false);
	// init cc
	if (dma_channel_is_claimed(MOD_PIEZO_DMA_CH_CC)) utils_error_mode(29);
	dma_channel_claim(MOD_PIEZO_DMA_CH_CC);
	dma_channel_config dma_cc_c = dma_channel_get_default_config(MOD_PIEZO_DMA_CH_CC);
	channel_config_set_transfer_data_size(&dma_cc_c, DMA_SIZE_32);
	channel_config_set_read_increment(&dma_cc_c, false);
	channel_config_set_write_increment(&dma_cc_c, false);
	channel_config_set_dreq(&dma_cc_c, DREQ_FORCE);
	dma_channel_configure(MOD_PIEZO_DMA_CH_CC, &dma_cc_c, &pwm_hw->slice[slice].cc, buffer_cc, 1, false);
	sleep_ms(1);

	clock_freq_hz = clock_get_hz(clk_sys);
}

void piezo_play(const piezo_anim_t anim) {
	// if (!state.piezo.busy)
	state.piezo.anim = anim;
}

static void set_pwm_freq(float freq_hz) {
	u32 top = freq_hz == 0.f ? 0 : (clock_freq_hz / (75.f * freq_hz)) - 1;
	u32 cc = top / 2;

	buffer_top[0] = top;
	buffer_cc[0] = cc;
	dma_channel_transfer_from_buffer_now(MOD_PIEZO_DMA_CH_CC, buffer_cc, 1);
	dma_channel_transfer_from_buffer_now(MOD_PIEZO_DMA_CH_TOP, buffer_top, 1);
}

static void anim_off(const bool is_off) {
	static bool off = false;

	if (is_off == off) return;

	off = is_off;
	if (off) set_pwm_freq(0);
}

static void anim(const float freq, const u8 repeat, const u16 play_x10ms, const u16 pause_x10ms) {
	static bool init = false;
	static u8 cycle = 0;
	static u16 frame = 0;
	static u8 cycles = 0;

	if (!init) {
		if (state.piezo.busy) return;
		anim_off(false);
		cycle = 0;
		cycles = repeat;
		state.piezo.busy = true;
		init = true;
	}

	if (cycle == cycles) {
		init = false;
		anim_off(true);
		state.piezo.busy = false;
		state.piezo.anim = OFF;
		return;
	}

	if (frame < play_x10ms) set_pwm_freq(freq);
	else set_pwm_freq(0);

	frame = (frame + 1) % (play_x10ms + pause_x10ms);
	if (frame % (play_x10ms + pause_x10ms) == 0) {
		cycle++;
	}
}

static void anim_error() {
	anim(100.f, 2, 10, 7);
}

static void anim_short_ack() {
	anim(3050.f, 1, 4, 0);
}

static void anim_short_error() {
	anim(100.f, 1, 4, 0);
}

void piezo_animation() {
	switch (state.piezo.anim) {
		case OFF:
			anim_off(true);
			break;
		case ERROR:
			anim_error();
			break;
		case SHORT_ACK:
			anim_short_ack();
			break;
		case SHORT_ERROR:
			anim_short_error();
		break;
		default:
			break;
	}
}
