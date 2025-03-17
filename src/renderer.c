// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "renderer.h"

#include <stdlib.h>
#include <hardware/gpio.h>
#include <pico/time.h>

#include "state.h"
#include "utils.h"
#include "defines/config.h"
#include "modules/numbers/numbers.h"
#include "modules/piezo/piezo.h"
#include "modules/status/status.h"
#include "modules/switches/switches.h"
#include "shared_modules/mcp/mcp.h"

#if DBG
static u32 render_times[100] = { 0 };
static i32 render_times_i = 0;
#endif

static void set_state() {
	if (state.phase != PHASE_IDLE) return; // busy

	// Numbers module
	const auto num_btn_pressed = mcp_is_pin_low(MOD_NUM_BTN);
	if (state.numbers.btn_pressed != num_btn_pressed) {
		state.numbers.btn_pressed = num_btn_pressed;
		if (num_btn_pressed) numbers_generate_target();
	}
	// potential refactor...
	if (utils_time_diff_ms(state.numbers.last_encoder_change, time_us_32()) > MOD_NUM_ENC_DEBOUNCE_MS) {
		const auto num_enc1 = mcp_is_pin_low(MOD_NUM_ENC1);
		const auto num_enc2 = mcp_is_pin_low(MOD_NUM_ENC2);
		if (num_enc1 != num_enc2) {
			state.numbers.last_encoder_change = time_us_32(); // TODO: change in other paths for debounce?
			if (num_enc1 == true && num_enc2 == false) {
				numbers_dec();
				state.numbers.last_encoder_incrementing = false;
				state.numbers.last_encoder_decrementing = true;
			} else if (num_enc1 == false && num_enc2 == true) {
				numbers_inc();
				state.numbers.last_encoder_incrementing = true;
				state.numbers.last_encoder_decrementing = false;
			}
		} else if (num_enc1 == true && num_enc2 == true) {
			state.numbers.last_encoder_change = time_us_32();
			if (state.numbers.last_encoder_incrementing) numbers_inc();
			else if (state.numbers.last_encoder_decrementing) numbers_dec();
		} else {
			state.numbers.last_encoder_incrementing = false;
			state.numbers.last_encoder_decrementing = false;
		}
	}
	state.status.numbers_on = state.numbers.number == state.numbers.target ? 1 : 0;

	// Switches module
	state.switches.switch1_on = mcp_is_pin_low(MOD_SWITCHES_SWITCH1);
	state.switches.switch2_on = mcp_is_pin_low(MOD_SWITCHES_SWITCH2);
	state.switches.switch_position = switches_get_position();

	// Launch module
	state.launch.pressed = mcp_is_pin_low(MOD_LAUNCH_BTN);
}

static void render_state() {
	bool render_status = false;

	// Numbers module
	if (state.numbers.number != current_state.numbers.number || state.numbers.target != current_state.numbers.target) {
		current_state.numbers.number = state.numbers.number;
		current_state.numbers.target = state.numbers.target;
		numbers_display(state.numbers.number, state.numbers.target);
		const bool on = state_get_bool(state.status.numbers_on);
		mcp_set_out(MOD_NUM_LED_G, on);
		mcp_set_out(MOD_NUM_LED_R, !on);
	}

	// Switches module
	if (state.switches.switch1_on != current_state.switches.switch1_on) {
		current_state.switches.switch1_on = state.switches.switch1_on;
		state_exit_minus_if_possible(&state.status.switches1_on, state.switches.switch1_on);
		state_set_bool_if_not_minus(&state.status.switches1_on, state.switches.switch1_on);
	}
	if (state.switches.switch2_on != current_state.switches.switch2_on) {
		current_state.switches.switch2_on = state.switches.switch2_on;
		state_exit_minus_if_possible(&state.status.switches2_on, state.switches.switch2_on);
		state_set_bool_if_not_minus(&state.status.switches2_on, state.switches.switch2_on);
	}
	if (state.switches.switch_position != current_state.switches.switch_position) {
		current_state.switches.switch_position = state.switches.switch_position;
		state.status.switches3_on = state.switches.switch_position == state.switches.target_position ? 1 : 0;
		switches_manage_leds();
	}

	// Status module
	if (state.status.switches1_on != current_state.status.switches1_on) {
		render_status = true;
		current_state.status.switches1_on = state.status.switches1_on;
		if (current_state.status.switches1_on && state.phase == PHASE_IDLE) piezo_play(PIEZO_SHORT_ACK);
		const bool on = state_get_bool(state.status.switches1_on);
		mcp_set_out(MOD_SWITCHES_LED1_R, !on);
		mcp_set_out(MOD_SWITCHES_LED1_G, on);
	}
	if (state.status.switches2_on != current_state.status.switches2_on) {
		render_status = true;
		current_state.status.switches2_on = state.status.switches2_on;
		if (current_state.status.switches2_on && state.phase == PHASE_IDLE) piezo_play(PIEZO_SHORT_ACK);
		const bool on = state_get_bool(state.status.switches2_on);
		mcp_set_out(MOD_SWITCHES_LED2_R, !on);
		mcp_set_out(MOD_SWITCHES_LED2_G, on);
	}
	if (state.status.switches3_on != current_state.status.switches3_on) {
		render_status = true;
		current_state.status.switches3_on = state.status.switches3_on;
		if (current_state.status.switches3_on && state.phase == PHASE_IDLE) piezo_play(PIEZO_SHORT_ACK);
		const bool on = state.switches.switch_position == state.switches.target_position;
		mcp_set_out(MOD_SWITCHES_LED3_G, on);
		mcp_set_out(MOD_SWITCHES_LED3_R, !on);
	}
	if (state.status.numbers_on != current_state.status.numbers_on) {
		render_status = true;
		current_state.status.numbers_on = state.status.numbers_on;
		if (current_state.status.numbers_on && state.phase == PHASE_IDLE) piezo_play(PIEZO_SHORT_ACK);
	}
	if (render_status) status_render_leds();

	if (state.launch.pressed != current_state.launch.pressed) {
		current_state.launch.pressed = state.launch.pressed;
		if (state.launch.pressed) {
			if (state_all_ok()) {
				state.phase = PHASE_COUNTDOWN;
			} else {
				state.phase = PHASE_ERROR;
			}
		}
	}
}

static void (**animation_fns)();
static u8 animation_fn_count;

void renderer_init(void (*animation_functions[])(), u8 animation_function_count) {
	animation_fns = animation_functions;
	animation_fn_count = animation_function_count;

	set_state();
	sleep_ms(1);

	// Switches module initial render
	current_state.switches.switch1_on = state.switches.switch1_on;
	mcp_set_out(MOD_SWITCHES_LED1_R, true);
	mcp_set_out(MOD_SWITCHES_LED1_G, false);
	current_state.switches.switch2_on = state.switches.switch2_on;
	mcp_set_out(MOD_SWITCHES_LED2_R, true);
	mcp_set_out(MOD_SWITCHES_LED2_G, false);
	mcp_set_out(MOD_SWITCHES_LED3_R, true);
	mcp_set_out(MOD_SWITCHES_LED3_G, false);
	switches_manage_leds();
}

[[noreturn]] void renderer_loop() {
#if DBG
	static int64_t acc_elapsed_us = 0;
#endif

	static u16 anim_frame = 0;

	// ReSharper disable once CppDFAEndlessLoop
	for (;;) {
		// ------------ start
		const auto start = time_us_32();

		// ------------ work
		set_state();
		render_state();
		for (u8 i = 0; i < animation_fn_count; i++) animation_fns[i]();

		// ------------ end
		anim_frame = (anim_frame + 1) % ANIM_FRAME_COUNT;
		utils_internal_led(anim_frame % 100 == 0);

		const auto end = time_us_32();
		const auto elapsed_us = utils_time_diff_us(start, end);
		const u32 remaining_us = elapsed_us > RENDER_TICK ? 0 : RENDER_TICK - elapsed_us;

#if DBG
		render_times[render_times_i] = elapsed_us;
		render_times_i = (render_times_i + 1) % 100;
		acc_elapsed_us += (remaining_us + elapsed_us);

		if (acc_elapsed_us >= 1 * 1'000'000) { // every 5 seconds
			// render averages out

			auto average = utils_avg(render_times, 100);
			const float elapsed_ms = elapsed_us / 1000.0f;
			utils_printf("render took: %.2f ms (%.0f us)\n", average / 1000.0f, average);
			utils_print_onboard_temp();

			// memory test
			size_t size = 491 * 1024;
			void *buffer = malloc(size);
			if (buffer == NULL) {
				printf("Allocation of %zu bytes failed\n", size);
			} else {
				printf("Allocation succeeded (%d kB)\n", size / 1024);
				free(buffer);
			}

			acc_elapsed_us = 0;
		}
#endif

		if (remaining_us > 0) sleep_us(remaining_us);
#if !DBG
		else utils_error_mode(66); // shouldn't even take 1 ms, let alone 10, something's wrong...
#endif
	}
}
