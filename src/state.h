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
	EXPLOSION,
	DARKNESS
} phase_t;

typedef enum {
	OFF,
	ERROR,
	SHORT_ACK,
	SHORT_ERROR,
} piezo_anim_t;

// FUNCTIONS

void state_set_0_if_possible(i8 *number);

bool state_get_bool(i8 number);

void state_set_bool_if_possible(i8 *number, bool val);

void state_set_minus();

// STATES

typedef struct {
	struct {
		i8 target;
		i8 number;
		bool btn_pressed;
		u32 last_encoder_change;
		bool last_encoder_incrementing;
		bool last_encoder_decrementing;
	} numbers;

	struct {
		i8 numbers_on;
	} status;

	struct {
		piezo_anim_t anim;
		bool busy;
		float freq;
	} piezo;

	phase_t phase;

	struct {
		bool dbg_btn;
		u32 last_encoder_change;
		bool last_encoder_incrementing;
		bool last_encoder_decrementing;
	} debug;
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
