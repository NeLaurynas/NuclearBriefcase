// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "utils.h"

#include <stdio.h>
#include <hardware/adc.h>
#include <hardware/clocks.h>

#include "defines/config.h"
#include "pico/rand.h"

u32 utils_random_in_range(u32 fromInclusive, u32 toInclusive) {
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

float utils_calculate_pio_clk_div(const float instruction_execution_in_us) {
	const auto frequency_hz = clock_get_hz(clk_sys);
	auto const clk_div = ((float)frequency_hz * instruction_execution_in_us) / 1'000'000.0f;
	return clk_div;
}

float utils_calculate_pio_clk_div_ns(const float instruction_execution_in_ns) {
	const auto frequency_hz = clock_get_hz(clk_sys);
	const auto clk_div = ((float)frequency_hz * instruction_execution_in_ns) / 1'000'000'000.0f;
	return clk_div;
}

inline int32_t utils_time_diff_ms(const u32 start_us, const u32 end_us) {
	return (int32_t)(end_us - start_us) / 1000;
}

int32_t utils_time_diff_us(u32 start_us, u32 end_us) {
	return (int32_t)(end_us - start_us);
}

void utils_error_mode(u8 code) {
	utils_internal_led(false);
	u8 long_blink = code / 10;
	u8 short_blink = code % 10;
	for (;;) {
		for (u8 i = 0; i < long_blink; i++) {
			utils_internal_led(true);
			sleep_ms(500);
			utils_internal_led(false);
			sleep_ms(500);
		}
		sleep_ms(400);
		for (u8 i = 0; i < short_blink; i++) {
			utils_internal_led(true);
			sleep_ms(75);
			utils_internal_led(false);
			sleep_ms(700);
		}
		sleep_ms(3'000); // sleep for 3 seconds
	}
}

void utils_internal_led(const bool on) {
	gpio_put(INTERNAL_LED, on);
}

u16 utils_proportional_reduce(u16 number, u16 step, u16 total_steps) {
	if (step >= total_steps) step = total_steps;
	return (float)number / total_steps * step;
}
