// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "renderer.h"

#include <pico/time.h>
#include <pico/types.h>

#include "state.h"
#include "utils.h"
#include "defines/config.h"
#include "modules/mcp/mcp.h"
#include "modules/numbers/numbers.h"
#include "modules/status/status.h"
#include "modules/wsleds/wsleds.h"

void set_state() {
	// Numbers module
	const auto num_btn_pressed = mcp_is_pin_low(MOD_NUM_BTN);
	if (state.numbers.btn_pressed != num_btn_pressed) {
		state.numbers.btn_pressed = num_btn_pressed;
		if (num_btn_pressed) {
			numbers_generate_target();
		}
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

	// Status module
	state.status.numbers_on = state.numbers.target == state.numbers.number;
}

void render_state() {
	// Numbers module
	if (state.numbers.number != currentState.numbers.number || state.numbers.target != currentState.numbers.target) {
		currentState.numbers.number = state.numbers.number;
		currentState.numbers.target = state.numbers.target;

		numbers_display(state.numbers.number, state.numbers.target);
		numbers_ok(state.numbers.number == state.numbers.target);
	}

	// Status module
	if (state.status.numbers_on != currentState.status.numbers_on) {
		currentState.status.numbers_on = state.status.numbers_on;
		status_set_on(MOD_STAT_LED_NUMBERS, state.status.numbers_on);
	}
}

void renderer_loop() {
#if DBG
	int64_t acc_elapsed_us = 0;
#endif

	u16 anim_frame = 0;
	bool test = true;

	for (;;) {
		// ------------ start
		const auto start = time_us_32();

		// ------------ work
		set_state();
		render_state();
		wsleds_anim_flag();

		// ------------ end
		anim_frame = (anim_frame + 1) % 1000;
		utils_internal_led(anim_frame % 100 == 0);

		auto end = time_us_32();
		auto elapsed_us = utils_time_diff_us(start, end);
		auto remaining_us = RENDER_TICK - elapsed_us;

#if DBG
		acc_elapsed_us += (remaining_us + elapsed_us);

		if (acc_elapsed_us >= 10 * 1'000'000) { // 10 seconds
			wsleds_test(); // actually looks better lol
			if (test == true) {
				test = false;
			} else {
				test = true;
			}
			const float elapsed_ms = elapsed_us / 1000.0f;
			printf("render took: %.2f ms (%ld us)\n", elapsed_ms, elapsed_us);
			utils_print_onboard_temp();
			acc_elapsed_us = 0;
			// recalculate because printf is slow
			end = time_us_32();
			elapsed_us = utils_time_diff_us(start, end);
			remaining_us = RENDER_TICK - elapsed_us;
		}
#endif

		if (remaining_us > 0) sleep_us(remaining_us);
	}
}
