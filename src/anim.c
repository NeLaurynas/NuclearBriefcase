// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "anim.h"

#include <math.h>

#include "utils.h"

void adjust_frame_by_speed_freq(const u16 frame, const u16 frame_count, const float speed,
						const float freq, u16 *divisor, float *adjusted_frame) {
	*divisor = frame_count / freq;
	*adjusted_frame = fmod(frame, *divisor) * speed;
}

u8 anim_color_reduction(const anim_direction_t direction, const u16 frame, const u16 frame_count, const float speed,
                        const float freq) {

	float adjusted_frame;
	u16 divisor;
	adjust_frame_by_speed_freq(frame, frame_count, speed, freq, &divisor, &adjusted_frame);

	u8 reduction = utils_proportional_reduce(255, adjusted_frame, divisor);

	if (direction == TO_BRIGHT) reduction = 255 - reduction;
	// TODO: PULSE

	return reduction;
}

u32 anim_color_blend(u32 color_from, u32 color_to, u16 frame, u16 frame_count, float speed, float freq) {
	float adjusted_frame;
	u16 divisor;
	adjust_frame_by_speed_freq(frame, frame_count, speed, freq, &divisor, &adjusted_frame);

	// todo: blend
	return 0;
}
