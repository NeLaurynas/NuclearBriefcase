// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef ANIM_H
#define ANIM_H
#include "defines/config.h"

typedef enum {
	TO_BRIGHT,
	TO_DIM,
	PULSE
} anim_direction_t;

/**
 * Reduces color brightness
 *
 * @param speed  How fast animation will go to an end (will hold)
 * @param freq How many times in frame_ticks it will get repeated
 */
u8 anim_color_reduction(anim_direction_t direction, u16 frame, u16 frame_ticks, float speed,  float freq);

#endif //ANIM_H
