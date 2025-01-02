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

void set_state() {
	// Numbers module
	auto num_btn_pressed = mcp_is_pin_low(MOD_NUM_BTN);
	if (state.numbers.btn_pressed != num_btn_pressed) {
		state.numbers.btn_pressed = num_btn_pressed;
		if (num_btn_pressed) {
			numbers_generate_target();
		}
	}

	// todo: cache mcp_is_pin_low based on mcp no and bank, because each read is 130 microseconds! cache for 5 miliseconds?
	if (utils_time_diff_ms(state.numbers.last_encoder_change, time_us_32()) > MOD_NUM_ENC_DEBOUNCE_MS) {
		auto num_enc1 = mcp_is_pin_low(MOD_NUM_ENC1);
		auto num_enc2 = mcp_is_pin_low(MOD_NUM_ENC2);
		if (num_enc1 != num_enc2) { // discard 1 and 1, can't tell direction (0 and 0 also discarded - not spinning)
			state.numbers.last_encoder_change = time_us_32();
			if (num_enc1 == true && num_enc2 == false) {
				numbers_dec();
				state.numbers.last_encoder_incrementing = false;
				state.numbers.last_encoder_decrementing = true;
			}
			else if (num_enc1 == false && num_enc2 == true) {
				numbers_inc();
				state.numbers.last_encoder_incrementing = true;
				state.numbers.last_encoder_decrementing = false;
			}
		// wtf is this crap...
		} else if (num_enc1 == true && num_enc2 == true) {
			if (state.numbers.last_encoder_incrementing) numbers_inc();
			else if (state.numbers.last_encoder_decrementing) numbers_dec();
		} else {
			state.numbers.last_encoder_incrementing = false;
			state.numbers.last_encoder_decrementing = false;
		}
	}
}

void render_state() {
	// Numbers module
	if (state.numbers.number != currentState.numbers.number || state.numbers.target != currentState.numbers.target) {
		utils_printf("Numbers state [number/target] changed, rendering\n");
		currentState.numbers.number = state.numbers.number;
		currentState.numbers.target = state.numbers.target;

		numbers_display(state.numbers.number, state.numbers.target);
		numbers_ok(state.numbers.number == state.numbers.target);
	}
}

void renderer_loop() {
#if DBG
	int64_t acc_elapsed_us = 0;
#endif

	for (;;) {
		// ------------ start
		const absolute_time_t start = get_absolute_time();

		// ------------ work
		set_state();
		render_state();

		// ------------ end
		auto end = get_absolute_time();
		auto elapsed_us = absolute_time_diff_us(start, end);
		auto remaining_us = RENDER_TICK - elapsed_us;

#if DBG
		acc_elapsed_us += (remaining_us + elapsed_us);

		if (acc_elapsed_us >= 10 * 1'000'000) { // 10 seconds
			float elapsed_ms = elapsed_us / 1000.0f;
			printf("render took: %.2f ms (%lld us)\n", elapsed_ms, elapsed_us);
			utils_print_onboard_temp();
			acc_elapsed_us = 0;
			// recalculate because printf is slow
			end = get_absolute_time();
			elapsed_us = absolute_time_diff_us(start, end);
			remaining_us = RENDER_TICK - elapsed_us;
		}
#endif

		if (remaining_us > 0) sleep_us(remaining_us);
	}
}
