// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "state.h"

State state = {
	.status = {
		.switches1_on = -1,
		.switches2_on = -1,
		.switches3_on = -1,
		.numbers_on = -1,
	},

	.piezo = {
		.anim = PIEZO_OFF,
		.prev_anim = PIEZO_OFF,
	},

	.launch = {
		.anim = LAUNCH_OFF,
	},

	.phase = PHASE_IDLE,
};

CurrentState current_state = { 0 };

inline bool state_get_bool(const i8 number) {
	return number > 0;
}

inline void state_exit_minus_if_possible(i8 *number, const bool val) {
	if (val) *number = 1;
}

inline void state_set_bool_if_not_minus(i8 *number, const bool val) {
	if (*number < 0) return;
	*number = val ? 1 : 0;
}

void state_set_minus() {
	// we regenerate numbers and switch position anyway, they will change to red state
	state.status.switches1_on = -1;
	state.status.switches2_on = -1;
	state.status.switches3_on = -1;
	state.status.numbers_on = -1;
}

inline bool state_all_ok() {
	return state_get_bool(state.status.numbers_on) && state_get_bool(state.status.switches1_on) &&
		state_get_bool(state.status.switches2_on) && state_get_bool(state.status.switches3_on);
}
