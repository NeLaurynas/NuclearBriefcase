// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "anim.h"

#include "utils.h"

u8 anim_color_reduction(const anim_direction_t direction, const u16 frame, const u16 frame_size, float speed, const float freq) {
	// 255 - : from bright to low, otherwise to low to bright
	// frame * X : how fast to reduce light
	// const u8 reduction = 255 - utils_proportional_reduce(255, ((frame % 500) * 5), FRAME_SIZE / 2);
	speed = speed * freq;
	u16 divisor = frame_size / freq;
	// utils_printf("frame size - %d, divisor: %d, frame = %d, divided: %d\n", frame_size, divisor, frame, frame % divisor);
	u8 reduction = utils_proportional_reduce(255, ((frame) % divisor * speed), frame_size / freq);

	if (direction == TO_DIM) reduction = 255 - reduction;
	// TODO: PULSE

	return reduction;
}
