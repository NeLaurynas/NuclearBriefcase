// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "piezo.h"

#include <string.h>
#include <hardware/clocks.h>
#include <hardware/dma.h>
#include <hardware/gpio.h>
#include <hardware/pwm.h>

#include "piezo_data.h"
#include "state.h"
#include "utils.h"
#include "defines/config.h"

static uint slice = 0;
static uint channel = 0;
static u32 clock_freq_hz = 0;
static u32 buffer_top[1] = { 0 };
static u32 buffer_cc[1] = { 0 };

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
	auto dma_top_c = dma_channel_get_default_config(MOD_PIEZO_DMA_CH_TOP);
	channel_config_set_transfer_data_size(&dma_top_c, DMA_SIZE_32);
	channel_config_set_read_increment(&dma_top_c, false);
	channel_config_set_write_increment(&dma_top_c, false);
	channel_config_set_dreq(&dma_top_c, DREQ_FORCE);
	dma_channel_configure(MOD_PIEZO_DMA_CH_TOP, &dma_top_c, &pwm_hw->slice[slice].top, buffer_top, 1, false);
	// init cc
	if (dma_channel_is_claimed(MOD_PIEZO_DMA_CH_CC)) utils_error_mode(29);
	dma_channel_claim(MOD_PIEZO_DMA_CH_CC);
	auto dma_cc_c = dma_channel_get_default_config(MOD_PIEZO_DMA_CH_CC);
	channel_config_set_transfer_data_size(&dma_cc_c, DMA_SIZE_32);
	channel_config_set_read_increment(&dma_cc_c, false);
	channel_config_set_write_increment(&dma_cc_c, false);
	channel_config_set_dreq(&dma_cc_c, DREQ_FORCE);
	dma_channel_configure(MOD_PIEZO_DMA_CH_CC, &dma_cc_c, &pwm_hw->slice[slice].cc, buffer_cc, 1, false);
	sleep_ms(1);

	clock_freq_hz = clock_get_hz(clk_sys);
}

void piezo_play(const piezo_anim_t anim) {
	if (!state.piezo.busy) state.piezo.anim = anim;
}

static void set_pwm_freq(float freq_hz) {
	const u32 top = freq_hz == 0.f ? 0 : (clock_freq_hz / (75.f * freq_hz)) - 1;
	const u16 cc = top / 2;

	buffer_top[0] = top;
	buffer_cc[0] = channel == 1 ? cc << 16 : cc;
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
		state.piezo.anim = PIEZO_OFF;
		return;
	}

	if (frame < play_x10ms && frame == 0) set_pwm_freq(freq);
	else if (frame == 0) set_pwm_freq(0);

	frame = (frame + 1) % (play_x10ms + pause_x10ms);
	if (frame % (play_x10ms + pause_x10ms) == 0) {
		cycle++;
	}
}

static void anim_melody(const char *melody, const float pause, const float pace) {
	static bool init = false;
	static u8 cycle = 0;
	static u16 frame = 0;
	static u16 cycles = 0;
	static u16 i = 0;
	static u16 next_trigger_frame = 0;
	static bool next_trigger_pause = false;

	if (!init) {
		if (state.piezo.busy) return;
		anim_off(false);
		cycle = 0;
		i = 0;
		cycles = strlen(melody) / 5;
		state.piezo.busy = true;
		init = true;
		frame = 0;
		next_trigger_frame = 0;
		next_trigger_pause = false;
	}

	if (cycle == cycles && frame == next_trigger_frame) {
		utils_printf("frame: %d\n", frame);
		init = false;
		anim_off(true);
		state.piezo.busy = false;
		state.piezo.anim = PIEZO_OFF;
		return;
	}

	const char tens = melody[i + 3];
	const char ones = melody[i + 4];
	const u8 length = (tens - '0') * 10 + (ones - '0');

	if (frame == next_trigger_frame && !next_trigger_pause) {
		const char note = melody[i];
		const char accidental = melody[i + 1];
		const char octave = melody[i + 2];
		const float frequency = piezo_data_get_frequency(note, accidental, octave);

		next_trigger_frame = frame + (pace/(float)length) * 100;
		next_trigger_pause = true;

		cycle++;
		i += 5; // one note's length is 5

		set_pwm_freq(frequency);
	} else if (frame == next_trigger_frame && next_trigger_pause) {
		set_pwm_freq(0);
		next_trigger_frame = frame + ((pace/(float)length) * 100) * pause;
		next_trigger_pause = false;
	}

	if (next_trigger_frame != frame) frame++;
}

static void anim_error() {
	anim(100.f, 2, 10, 7);
}

static void anim_short_ack() {
	anim(3050.f, 1, 4, 0);
}

static void anim_short_error() {
	anim(100.f, 1, 10, 0);
}

static void anim_underworld() {
	const char *melody = "Cn412Cn512An312An412As312As412Oxx06Oxx03Cn412Cn512An312An412As312As412Oxx06Oxx03Fn312Fn412Dn312Dn412Ds312Ds412Oxx06Oxx03Fn312Fn412Dn312Dn412Ds312Ds412Oxx06Oxx06Ds418Cs418Dn418Cs406Ds406Ds406Gs306Gn306Cs406Cn418Fs418Fn418En318As418An418Gs410Ds410Bn310As310An310Gs310Oxx03Oxx03Oxx03";
	anim_melody(melody, 1.3f, .8f);
}

static void anim_crazy_frog() {
	const char *melody = "An402Cn504An404An408Dn504An404Gn404An402En504An404An408Fn504En504Cn504An404En504An504An408Gn404Gn408En404Bn404An401Oxx04An402Cn504An404An408Dn504An404Gn404An402En504An404An408Fn504En504Cn504An404En504An504An408Gn404Gn408En404Bn404An401Oxx04An308Gn304En304Dn304An402Cn504An404An408Dn504An404Gn404An402En504An404An408Fn504En504Cn504An404En504An504An408Gn404Gn408En404Bn404An401";
	anim_melody(melody, .3f, .9f);
}

void piezo_animation() {
	switch (state.piezo.anim) {
		case PIEZO_OFF:
			anim_off(true);
			break;
		case PIEZO_ERROR:
			anim_error();
			break;
		case PIEZO_SHORT_ACK:
			anim_short_ack();
			break;
		case PIEZO_SHORT_ERROR:
			anim_short_error();
			break;
		case MUSIC_UNDERWORLD:
			anim_underworld();
			break;
		case MUSIC_CRAZY_FROG:
			anim_crazy_frog();
			break;
		default:
			break;
	}
}
