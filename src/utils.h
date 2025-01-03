// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

#include "defines/config.h"

u32 util_random_in_range(u32 fromInclusive, u32 toInclusive);

float utils_print_onboard_temp();

void utils_print_cpu_speed();

float utils_calculate_pio_clk_div(float instruction_execution_in_us);

float utils_calculate_pio_clk_div_ns(float instruction_execution_in_ns);

void utils_printf(const char *format, ...);

int32_t utils_time_diff_ms(u32 start_us, u32 end_us);

int32_t utils_time_diff_us(u32 start_us, u32 end_us);

void utils_internal_led(bool on);

#endif //UTILS_H
