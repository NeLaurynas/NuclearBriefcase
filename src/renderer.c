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
	int64_t acc_elapsed_us = 0;

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
