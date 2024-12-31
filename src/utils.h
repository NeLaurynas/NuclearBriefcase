// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

uint32_t util_random_in_range(uint32_t fromInclusive, uint32_t toInclusive) ;
float utils_print_onboard_temp();
void utils_print_cpu_speed();

#endif //UTILS_H
