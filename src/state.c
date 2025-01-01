// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "state.h"

State state = {
	.numbers = {
		.target = 0,
		.number = 0,
		.btn_pressed = false,
	},
	.another_value = 0
};

State currentState = {};
