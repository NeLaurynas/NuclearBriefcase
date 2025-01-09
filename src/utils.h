// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

#include "defines/config.h"

u32 utils_random_in_range(u32 fromInclusive, u32 toInclusive);

float utils_print_onboard_temp();

void utils_print_cpu_speed();

float utils_calculate_pio_clk_div(float instruction_execution_in_us);

float utils_calculate_pio_clk_div_ns(float instruction_execution_in_ns);

#if defined(DBG) && DBG
#define utils_printf(...) printf(__VA_ARGS__)
#else
#define utils_printf(...) (void)0
#endif

int32_t utils_time_diff_ms(u32 start_us, u32 end_us);

int32_t utils_time_diff_us(u32 start_us, u32 end_us);

void utils_error_mode(u8 code);

void utils_internal_led(bool on);

u16 utils_proportional_reduce(u16 number, u16 step, u16 total_steps);

#endif //UTILS_H
