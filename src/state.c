// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "state.h"

State state = {
	.numbers = {
		.target = 0,
		.number = 0,
		.btn_pressed = 0,
		.last_encoder_change = 0,
		.last_encoder_incrementing = 0,
		.last_encoder_decrementing = 0,
	},
	.status = {
		.numbers_on = 0,
	},
	.another_value = 0
};

CurrentState currentState = {};
