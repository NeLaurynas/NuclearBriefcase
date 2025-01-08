// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include "defines/config.h"

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

	int another_value;
} State;

typedef struct {
	struct {
		i8 target;
		i8 number;
	} numbers;

	struct {
		i8 numbers_on;
	} status;

	int another_value;
} CurrentState;


extern State state;
extern CurrentState currentState;

#endif //STATE_H
