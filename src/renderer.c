// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "renderer.h"

#include <stdio.h>
#include <pico/time.h>
#include <pico/types.h>

#include "state.h"
#include "utils.h"

void renderer_loop() {
	const int64_t tick = 10000; // 10 ms in microseconds
	const bool debug = true;
	int64_t acc_elapsed_us = 0;

	for (;;) {
		// ------------ start
		const absolute_time_t start = get_absolute_time();

		// ------------ work
		// utils_print_onboard_temp();

		// ------------ end
		absolute_time_t end = get_absolute_time();
		int64_t elapsed_us = absolute_time_diff_us(start, end);
		int64_t remaining_us = tick - elapsed_us;
		acc_elapsed_us += (remaining_us + elapsed_us);

		if (debug && acc_elapsed_us >= 3 * 1000000) { // 5 seconds
			float elapsed_ms = elapsed_us / 1000.0f;
			printf("render took: %.2f ms (%lld us)\n", elapsed_ms, elapsed_us);
			utils_print_onboard_temp();
			printf("State: %i\n", currentState.numbers.target);
			acc_elapsed_us = 0;
			// recalculate because printf is slow
			end = get_absolute_time();
			elapsed_us = absolute_time_diff_us(start, end);
			remaining_us = tick - elapsed_us;
		}

		if (remaining_us > 0) sleep_us(remaining_us);
	}
}
