// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STATE_H
#define STATE_H

#include <stdint.h>

#include "shared_config.h"

// ENUMS

typedef enum {
	PHASE_IDLE,
	PHASE_ERROR,
	PHASE_COUNTDOWN,
	PHASE_EXPLOSION,
	PHASE_DARKNESS
} phase_t;

typedef enum {
	PIEZO_OFF,
	PIEZO_ERROR,
	PIEZO_SHORT_ACK,
	PIEZO_SHORT_ERROR,
	PIEZO_CUSTOM,
	MUSIC_UNDERWORLD,
	MUSIC_CRAZY_FROG,
} piezo_anim_t;

typedef enum {
	LAUNCH_OFF,
	LAUNCH_ERROR,
	LAUNCH_PULSE
} launch_anim_t;

// FUNCTIONS

void state_set_0_if_needed(i8 *number);

bool state_get_bool(i8 number);

void state_set_bool_if_possible(i8 *number, bool val);

void state_set_minus();

// STATES

typedef struct {
	struct {
		u32 last_encoder_change;
		i8 target;
		i8 number;
		bool btn_pressed;
		bool last_encoder_incrementing;
		bool last_encoder_decrementing;
	} numbers;

	struct {
		i8 numbers_on;
	} status;

	struct {
		bool pressed;
		launch_anim_t anim;
	} launch;

	struct {
		piezo_anim_t anim;
		piezo_anim_t prev_anim;
		float freq;
	} piezo;

	struct {
		i8 switch1_on;
		i8 switch2_on;
	} tumbler;

	phase_t phase;

	bool dbg_pressed;
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
		bool pressed;
	} launch;

	struct {
		bool on_target;
	} wsleds;

	bool dbg_pressed;
} CurrentState;

extern State state;
extern CurrentState current_state;

#endif //STATE_H
