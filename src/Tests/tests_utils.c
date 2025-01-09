// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "tests_utils.h"

#include <CUnit/CUnit.h>

// copy and paste is easier instead of managing ARM specific headers....
static __uint16_t utils_proportional_reduce(const __uint16_t number, __uint16_t step, const __uint16_t total_steps) {
	if (step >= total_steps) step = total_steps;
	return (float)number / total_steps * step;
}

void test_utils_proportional_reduce() {
	CU_ASSERT_EQUAL(utils_proportional_reduce(10, 1, 10), 1);
	CU_ASSERT_EQUAL(utils_proportional_reduce(10, 2, 10), 2);
	CU_ASSERT_EQUAL(utils_proportional_reduce(10, 2, 5), 4);
	CU_ASSERT_EQUAL(utils_proportional_reduce(100, 25, 50), 50);
	CU_ASSERT_EQUAL(utils_proportional_reduce(10, 4, 3), 10);
	CU_ASSERT_EQUAL(utils_proportional_reduce(10, 3, 3), 10);
	CU_ASSERT_EQUAL(utils_proportional_reduce(1000, 4, 3), 1000);
	CU_ASSERT_EQUAL(utils_proportional_reduce(1000, 3, 3), 1000);
	CU_ASSERT_EQUAL(utils_proportional_reduce(10, 444, 333), 10);
	CU_ASSERT_EQUAL(utils_proportional_reduce(10, 333, 333), 10);
}
