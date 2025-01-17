// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

#include "defines/config.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr[0])) // will not work for decayed arrays

u8 utils_min_u8(u8 x, u8 y);
u16 utils_min_u16(u16 x, u16 y);
u32 utils_min_u32(u32 x, u32 y);
#define utils_min(x, y) \
	_Generic((x), \
		u8: utils_min_u8, \
		u16: utils_min_u16, \
		u32: utils_min_u32, \
		int: utils_min_u32 \
	)(x, y)

void utils_swap_u8(u8 *x, u8 *y);
void utils_swap_u16(u16 *x, u16 *y);
void utils_swap_u32(u32 *x, u32 *y);
#define utils_swap(x, y) \
_Generic((x), \
u8*: utils_swap_u8, \
u16*: utils_swap_u16, \
u32*: utils_swap_u32, \
int*: utils_swap_u32 \
)(x, y)

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
