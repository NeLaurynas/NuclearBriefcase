// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "renderer.h"

#include <stdlib.h>
#include <hardware/gpio.h>
#include <pico/time.h>

#include "state.h"
#include "utils.h"
#include "defines/config.h"
#include "modules/mcp/mcp.h"
#include "modules/numbers/numbers.h"
#include "modules/piezo/piezo.h"
#include "modules/status/status.h"

void set_state() {
	if (state.phase != IDLE) return; // busy

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
				state_set_0_if_possible(&state.status.numbers_on);
				state.numbers.last_encoder_incrementing = false;
				state.numbers.last_encoder_decrementing = true;
			} else if (num_enc1 == false && num_enc2 == true) {
				numbers_inc();
				state_set_0_if_possible(&state.status.numbers_on);
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
	state_set_bool_if_possible(&state.status.numbers_on, state.numbers.target == state.numbers.number);

	// potential refactor...
	if (utils_time_diff_ms(state.debug.last_encoder_change, time_us_32()) > MOD_NUM_ENC_DEBOUNCE_MS) {
		const bool num_enc1 = !gpio_get(DBG_ENC1);
		const bool num_enc2 = !gpio_get(DBG_ENC2);
		if (num_enc1 != num_enc2) {
			state.debug.last_encoder_change = time_us_32(); // TODO: change in other paths for debounce?
			if (num_enc1 == true && num_enc2 == false) {
				dec();
				state.debug.last_encoder_incrementing = false;
				state.debug.last_encoder_decrementing = true;
			} else if (num_enc1 == false && num_enc2 == true) {
				inc();
				state.debug.last_encoder_incrementing = true;
				state.debug.last_encoder_decrementing = false;
			}
		} else if (num_enc1 == true && num_enc2 == true) {
			state.debug.last_encoder_change = time_us_32();
			if (state.debug.last_encoder_incrementing) {
				inc();
			}
			else if (state.debug.last_encoder_decrementing) {
				dec();
			}
		} else {
			state.debug.last_encoder_incrementing = false;
			state.debug.last_encoder_decrementing = false;
		}
	}

	const bool dbg_btn_pressed = !gpio_get(DBG_BTN_PIN);
	if (state.debug.dbg_btn != dbg_btn_pressed) {
		state.debug.dbg_btn = dbg_btn_pressed;
		if (dbg_btn_pressed) {
			piezo_play(ERROR);
		}
	}
}

void render_state() {
	// Numbers module
	if (state.numbers.number != currentState.numbers.number || state.numbers.target != currentState.numbers.target) {
		currentState.numbers.number = state.numbers.number;
		currentState.numbers.target = state.numbers.target;

		numbers_display(state.numbers.number, state.numbers.target);
	}

	// Status module
	if (state.status.numbers_on != currentState.status.numbers_on) {
		currentState.status.numbers_on = state.status.numbers_on;
		status_set_on(MOD_STAT_LED_NUMBERS, state.status.numbers_on);
		numbers_ok(state_get_bool(state.status.numbers_on));
	}
	if (state.phase == IDLE && state.status.numbers_on == 1) {
		// if all systems green - go to next phase
		state.phase = COUNTDOWN;
	}

}

static void (**animation_fns)();
static u8 animation_fn_count;

void renderer_init(void (*animation_functions[])(), u8 animation_function_count) {
	animation_fns = animation_functions;
	animation_fn_count = animation_function_count;
}

void renderer_loop() {
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

		auto end = time_us_32();
		auto elapsed_us = utils_time_diff_us(start, end);
		auto remaining_us = RENDER_TICK - elapsed_us;

#if DBG
		acc_elapsed_us += (remaining_us + elapsed_us);

		if (acc_elapsed_us >= 10 * 1'000'000) { // 10 seconds
			const float elapsed_ms = elapsed_us / 1000.0f;
			utils_printf("render took: %.2f ms (%ld us)\n", elapsed_ms, elapsed_us);
			utils_print_onboard_temp();

			size_t allocated = 480 * 1024;
			// so 480 kb is free for sure
			char *ptr = malloc(allocated);
			if (ptr != NULL) { // seems to panic and not return null
				printf("Successfully allocated: %zu KB\n", allocated / 1024);
			} else {
				printf("Failed to allocate %zu KB\n", allocated / 1024);
				break;
			}
			free(ptr);
			printf("Free'd: %zu KB\n", allocated / 1024);

			acc_elapsed_us = 0;
			// recalculate because printf is slow
			end = time_us_32();
			elapsed_us = utils_time_diff_us(start, end);
			remaining_us = RENDER_TICK - elapsed_us;
		}
#endif

		if (remaining_us > 0) sleep_us(remaining_us);
#if !DBG
		else utils_error_mode(66); // shouldn't even take 1 ms, let alone 10, something's wrong...
#endif
	}
}
