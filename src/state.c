// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "state.h"

State state = {
	.status = {
		.numbers_on = -1,
	},

	.piezo = {
		.anim = PIEZO_OFF,
		.prev_anim = PIEZO_OFF,
	},

	.phase = PHASE_IDLE,
};

CurrentState current_state = { 0 };

void state_set_0_if_possible(i8 *number) {
	if (*number < 0) *number = 0;
}

inline bool state_get_bool(const i8 number) {
	return number > 0;
}

void state_set_bool_if_possible(i8 *number, const bool val) {
	if (*number < 0) return;
	*number = val;
}

void state_set_minus() {
	state.status.numbers_on = -1;
}
