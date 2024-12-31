// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "renderer.h"

#include <pico/time.h>
#include <pico/types.h>

#include "state.h"
#include "utils.h"
#include "defines/config.h"
#include "modules/mcp/mcp.h"

void renderer_loop() {
	int64_t acc_elapsed_us = 0;

	for (;;) {
		// ------------ start
		const absolute_time_t start = get_absolute_time();

		// ------------ work
		// utils_print_onboard_temp();

		// ------------ end
		absolute_time_t end = get_absolute_time();
		int64_t elapsed_us = absolute_time_diff_us(start, end);
		int64_t remaining_us = RENDER_TICK - elapsed_us;

#if DBG
		acc_elapsed_us += (remaining_us + elapsed_us);

		if (acc_elapsed_us >= 3 * 1'000'000) { // 3 seconds
			mcp_all();
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
