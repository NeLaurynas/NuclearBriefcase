// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "anim.h"

#include <math.h>

#include "utils.h"

u8 anim_color_reduction(const anim_direction_t direction, const u16 frame, const u16 frame_ticks, const float speed,
                        const float freq) {
	u16 divisor = frame_ticks / freq;
	float adjusted_frame = fmod(frame, divisor) * speed;

	u8 reduction = utils_proportional_reduce(255, adjusted_frame, divisor);

	if (direction == TO_BRIGHT) reduction = 255 - reduction;
	// TODO: PULSE

	return reduction;
}
