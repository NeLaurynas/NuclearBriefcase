// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include "defines/config.h"

// ENUMS

typedef enum {
	IDLE,
	COUNTDOWN,
	ROCKET,
	EXPLOSION,
	DARKNESS
} phase_t;

// FUNCTIONS

void state_set_0(i8 *number);

bool state_get_bool(i8 number);

void state_set_bool(i8 *number, bool val);

void state_set_minus();

// STATES

typedef struct {
	struct {
		u8 target;
		u8 number;
		bool btn_pressed;
		u32 last_encoder_change;
		bool last_encoder_incrementing;
		bool last_encoder_decrementing;
	} numbers;

	struct {
		i8 numbers_on;
	} status;

	phase_t phase;
} State;

typedef struct {
	struct {
		i8 target;
		i8 number;
	} numbers;

	struct {
		i8 numbers_on;
	} status;

	struct {
		bool on_target;
	} wsleds;
} CurrentState;

extern State state;
extern CurrentState currentState;

#endif //STATE_H
