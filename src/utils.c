// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "utils.h"

#include <stdio.h>
#include <hardware/adc.h>
#include <hardware/clocks.h>

#include "defines/config.h"
#include "pico/rand.h"

uint32_t util_random_in_range(uint32_t fromInclusive, uint32_t toInclusive) {
	if (fromInclusive > toInclusive) {
		const uint32_t tmp = toInclusive;
		toInclusive = fromInclusive;
		fromInclusive = tmp;
	}

	const uint32_t range = toInclusive - fromInclusive + 1; // +1 because to is inclusive
	const uint32_t rnd = get_rand_32();
	return fromInclusive + (rnd % range);
}

float utils_print_onboard_temp() {
	const float conversionFactor = 3.3f / (1 << 12);

	float adc = (float)adc_read() * conversionFactor;
	float tempC = 27.0f - (adc - 0.706f) / 0.001721f;

#if DBG
	printf("Onboard temperature = %.02f C\n", tempC);
#endif

	return tempC;
}

void utils_print_cpu_speed() {
#if DBG
	uint32_t freq_hz = clock_get_hz(clk_sys);

	float freq_mhz = (float)freq_hz / 1'000'000.0f;
	printf("System clock: %.2f MHz\n", freq_mhz);
#endif
}
