// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

uint32_t util_random_in_range(uint32_t fromInclusive, uint32_t toInclusive);

float utils_print_onboard_temp();

void utils_print_cpu_speed();

float utils_calculate_pio_clk_div(float instruction_execution_in_us);

float utils_calculate_pio_clk_div_ns(float instruction_execution_in_ns);

void utils_printf(const char *format, ...);

int32_t utils_time_diff_ms(uint32_t start_us, uint32_t end_us);

#endif //UTILS_H
