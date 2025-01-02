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
		uint32_t last_encoder_change;
		bool last_encoder_incrementing;
		bool last_encoder_decrementing;
	} numbers;

	struct {
		bool numbers_on;
	} status;

	int another_value;
} State;

typedef struct {
	struct {
		uint8_t target;
		uint8_t number;
	} numbers;

	struct {
		bool numbers_on;
	} status;

	int another_value;
} CurrentState;


extern State state;
extern CurrentState currentState;

#endif //STATE_H
