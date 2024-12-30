// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "utils.h"

#include <stdio.h>

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

void utils_display_bytes_as_binary(const void* data, size_t size) {
	const unsigned char* bytes = (const unsigned char*)data;
	for (size_t i = 0; i < size; i++) {
		for (int bit = 7; bit >= 0; bit--) { printf("%d", (bytes[i] >> bit) & 1); }
		printf(" ");
	}
	printf("\n");
}
