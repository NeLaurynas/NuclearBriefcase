// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "utils.h"

#include <stdarg.h>
#include <stdio.h>
#include <hardware/adc.h>
#include <hardware/clocks.h>

#include "defines/config.h"
#include "pico/rand.h"

uint32_t util_random_in_range(uint32_t fromInclusive, uint32_t toInclusive) {
	if (fromInclusive > toInclusive) {
		const auto tmp = toInclusive;
		toInclusive = fromInclusive;
		fromInclusive = tmp;
	}

	const auto range = toInclusive - fromInclusive + 1; // +1 because to is inclusive
	const auto rnd = get_rand_32();
	return fromInclusive + (rnd % range);
}

float utils_print_onboard_temp() {
	constexpr float conversionFactor = 3.3f / (1 << 12);

	const float adc = (float)adc_read() * conversionFactor;
	const float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

#if DBG
	printf("Onboard temperature = %.02f C\n", tempC);
#endif

	return tempC;
}

void utils_print_cpu_speed() {
#if DBG
	const auto freq_hz = clock_get_hz(clk_sys);

	const float freq_mhz = (float)freq_hz / 1'000'000.0f;
	printf("System clock: %.2f MHz\n", freq_mhz);
#endif
}

float utils_calculate_pio_clk_div(float instruction_execution_in_us) {
	const auto frequency_hz = clock_get_hz(clk_sys);

	const float clk_div = ((float)frequency_hz * instruction_execution_in_us) / 1'000'000.0f;

	return clk_div;
}

void utils_printf(const char* format, ...) {
#if DBG
	va_list args;
	va_start(args, format);

	vprintf(format, args);
	va_end(args);
#endif
}
