// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "utils.h"
#include "pico/rand.h"

#include <stdlib.h>

int util_random_in_range(int from, int to) {
	if (from > to) {
		const int tmp = to;
		to = from;
		from = tmp;
	}

	int range = to - from + 1; // +1 because to is inclusive
	uint32_t rnd = get_rand_32();
	return from + (rnd % range);
}
