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

// void state_set_0_if_possible(i8 *number);

bool state_get_bool(i8 number);

void state_exit_minus_if_possible(i8 *number, bool val);

void state_set_bool_if_not_minus(i8 *number, bool val);

void state_set_minus();

bool state_all_ok();

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
		i8 switches1_on;
		i8 switches2_on;
		i8 switches3_on;
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
		bool switch1_on;
		bool switch2_on;
		u8 switch_position;
		u8 target_position;
	} switches;

	phase_t phase;
} State;

typedef struct {
	struct {
		i8 target;
		i8 number;
	} numbers;

	struct {
		i8 numbers_on;
		i8 switches1_on;
		i8 switches2_on;
		i8 switches3_on;
	} status;

	struct {
		bool switch1_on;
		bool switch2_on;
		u8 switch_position;
	} switches;

	struct {
		bool pressed;
	} launch;

	struct {
		bool on_target;
	} wsleds;
} CurrentState;

extern State state;
extern CurrentState current_state;

#endif //STATE_H
