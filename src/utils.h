// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdio.h>

uint32_t util_random_in_range(int fromInclusive, int toInclusive);
void utils_display_bytes_as_binary(const void* data, size_t size);

#endif //UTILS_H
