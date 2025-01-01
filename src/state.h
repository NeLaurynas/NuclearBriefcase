// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STATE_H
#define STATE_H

#include <stdint.h>

typedef struct {
	struct {
		uint8_t target;
		uint8_t number;
		bool btn_pressed;
	} numbers;

	int another_value;
} State;

extern State state;
extern State currentState;

#endif //STATE_H
