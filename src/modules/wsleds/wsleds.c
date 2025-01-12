// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "wsleds.h"

#include <math.h>
#include <pio_wsleds.pio.h>
#include <stdlib.h>
#include <string.h>
#include <hardware/dma.h>
#include <hardware/pio.h>

#include "anim.h"
#include "state.h"
#include "utils.h"
#include "wsleds_data.h"
#include "defines/config.h"

static const u8 line_width = (u8)sqrt(MOD_WSLEDS_LED_COUNT);

u32 reduce_brightness(const u8 reduction, const u32 color) {
	u8 r = (color >> 16) & 0b11111111;
	u8 g = (color >> 8) & 0b11111111;
	u8 b = color & 0b11111111;

	r = (r > reduction) ? (r - reduction) : 0;
	g = (g > reduction) ? (g - reduction) : 0;
	b = (b > reduction) ? (b - reduction) : 0;

	return (r << 16) | (g << 8) | b;
}

static u32 buffer[MOD_WSLEDS_LED_COUNT] = { 0 };

void buffer_transfer() {
	dma_channel_transfer_from_buffer_now(MOD_WSLEDS_DMA_CH, buffer, MOD_WSLEDS_LED_COUNT);
}

void wsleds_init() {
	// init DMA
	if (dma_channel_is_claimed(MOD_WSLEDS_DMA_CH)) utils_error_mode(25);
	dma_channel_claim(MOD_WSLEDS_DMA_CH);
	dma_channel_config dma_c = dma_channel_get_default_config(MOD_WSLEDS_DMA_CH);
	channel_config_set_transfer_data_size(&dma_c, DMA_SIZE_32);
	channel_config_set_read_increment(&dma_c, true); // incr true - we loop through MOD_WSLEDS_LED_COUNT size buffer
	channel_config_set_write_increment(&dma_c, false);
	channel_config_set_dreq(&dma_c, pio_get_dreq(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, true));
	dma_channel_configure(MOD_WSLEDS_DMA_CH, &dma_c, &MOD_WSLEDS_PIO->txf[MOD_WSLEDS_SM], buffer, MOD_WSLEDS_LED_COUNT,
	                      false);
	sleep_ms(1);

	// get clock divider
	const auto clk_div = utils_calculate_pio_clk_div_ns(90);
	utils_printf("WSLEDS PIO CLK DIV: %f\n", clk_div);

	// init PIO
	const auto offset = pio_add_program(MOD_WSLEDS_PIO, &pio_wsleds_program);
	if (offset < 0) utils_error_mode(26);
	if (pio_sm_is_claimed(MOD_WSLEDS_PIO, MOD_WSLEDS_SM)) utils_error_mode(27);
	pio_sm_claim(MOD_WSLEDS_PIO, MOD_WSLEDS_SM);
	pio_wsleds_program_init(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, offset, MOD_WSLEDS_PIN, clk_div);
	pio_sm_set_enabled(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, true);
	sleep_ms(1);

	// buffer_transfer();
}

static inline u8 x_led_start(const u8 x_line) {
	return x_line * line_width;
}

static inline u8 get_line_x(const u8 dot) {
	return dot / line_width;
}

static inline u8 get_line_y(const u8 dot) {
	return dot % line_width;
}

static inline u8 get_led_from_lines(const u8 x_line, const u8 y_line) {
	return line_width * y_line + x_line;
}

void rotate_buffer_left(u8 times) {
	static u32 temp[MOD_WSLEDS_LED_COUNT] = { 0 };
	if (times == 0) return;

	for (u8 t = 0; t < times; t++) {
		for (u8 i = 0; i < 8; i++)
			for (u8 j = 0; j < 8; j++) temp[(7 - j) * 8 + i] = buffer[i * 8 + j];

		for (u8 i = 0; i < MOD_WSLEDS_LED_COUNT; i++) {
			buffer[i] = temp[i];
		}
	}
}

void anim_target() {
	static bool init = false;
	static u8 target_dot = 0;
	static u8 x_line = 0;
	static u8 y_line = 0;
	static u8 x_steps = 0;
	static u8 y_steps = 0;
	static u8 current_step = 0;
	static u16 frame = 1;
	static constexpr u16 FRAME_TICKS = 975;
	static constexpr u16 FRAME_TICK_DIVIDER = 75;
	static u8 freeze_frames = 0;

	if (!init) {
		target_dot = get_led_from_lines(utils_random_in_range(1, 6), utils_random_in_range(1, 6));
		x_steps = get_line_x(target_dot);
		y_steps = get_line_y(target_dot);
		init = true;
	}

	memset(buffer, 0, sizeof(buffer));

	// render target dot
	buffer[target_dot] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.5f, 10), COLOR_CYAN);

	// render X
	for (u8 i = 0; i < line_width; i++) {
		const u8 x_led = x_led_start(x_line) + i;
		const u8 y_led = y_line + i * line_width;
		if (x_line == get_line_x(target_dot) && y_line == get_line_y(target_dot)) {
			// on target - blink green
			currentState.wsleds.on_target = true;
			buffer[x_led] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.0f, 10), COLOR_GREEN);
			buffer[y_led] = buffer[x_led];
		} else {
			// blink dot over red lines
			currentState.wsleds.on_target = false;
			buffer[x_led] = get_line_x(x_led) == get_line_x(target_dot) && get_line_y(x_led) == get_line_y(target_dot)
				&&
				buffer[target_dot] != 0
				? buffer[target_dot]
				: COLOR_RED;
			buffer[y_led] = get_line_x(y_led) == get_line_x(target_dot) && get_line_y(y_led) == get_line_y(target_dot)
				&&
				buffer[target_dot] != 0
				? buffer[target_dot]
				: COLOR_RED;
		}
	}

	if (frame % FRAME_TICK_DIVIDER == 0) {
		if (get_line_x(target_dot) == x_line && get_line_y(target_dot) == y_line) {
			if (freeze_frames == 0) {
				freeze_frames = 6;
			}
			if (freeze_frames == 1) {
				target_dot = utils_random_in_range(0, 63);
				freeze_frames = 0;
			} else {
				freeze_frames--;
				goto end;
			}
			// calculate steps to take per frame for X
			x_steps = abs((i8)get_line_x(target_dot) - x_line);
			y_steps = abs((i8)get_line_y(target_dot) - y_line);
			current_step = 0;
			goto end;
		}
		if (x_steps != 0 || y_steps != 0) {
			const auto max_steps = (x_steps > y_steps ? x_steps : y_steps);
			if (current_step == max_steps) {
				frame = 0;
				x_steps = 0;
				y_steps = 0;
			} else {
				current_step++;

				x_line = (get_line_x(target_dot) < x_line)
					? x_line - 1
					: get_line_x(target_dot) == x_line
					? x_line
					: x_line + 1;
				y_line = (get_line_y(target_dot) < y_line)
					? y_line - 1
					: get_line_y(target_dot) == y_line
					? y_line
					: y_line + 1;
			}
		}
	}
end:
	frame = (frame + 1) % FRAME_TICKS;
	buffer_transfer();
}

void anim_countdown() {
	static bool init = false;
	static i8 number = 0;
	static u16 frame = 0;
	static constexpr u16 FRAME_TICKS = 100; // every second

	if (!init) {
		number = 9;
		frame = 0;
		init = true;
	}

	if (number == -2) {
		// deinit
		init = false;
		state.phase = EXPLOSION;
		memset(buffer, 0, sizeof(buffer));
		buffer_transfer();
		return;
	}

	if (frame % FRAME_TICKS == 0) {
		if (number >= 0) memcpy(buffer, NUMBERS[number], sizeof(buffer));
		number--;
	}
	for (u8 i = 0; i < MOD_WSLEDS_LED_COUNT; i++) { // why use led count config, when a lot assumes it's 8x8...
		if (buffer[i] == 0) continue;

		u32 color = COLOR_RED;
		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		switch (number + 1) {
			case 0:
			case 1:
			case 2:
				color = COLOR_GREEN;
				break;
			case 3:
			case 4:
			case 5:
				color = COLOR_YELLOW;
				break;
		}

		buffer[i] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.00f, 1), color);
	}

	buffer_transfer();
	frame = (frame + 1) % FRAME_TICKS;
}

inline void fill_ring_with_color(const u8 *ring, const u8 size, const u32 color) {
	for (u8 i = 0; i < size; i++) {
		buffer[ring[i]] = color;
	}
}

void anim_explosion() {
	static bool init = false;
	static i8 number = 0;
	static u8 rotation = 0;
	static u8 ring0_loc[1] = { 27 };
	static u8 ring1_loc[5] = { 19, 26, 28, 35, 36 };
	static u8 ring2_loc[9] = { 11, 18, 20, 25, 29, 34, 37, 43, 44 };
	static u8 ring3_loc[13] = { 3, 10, 12, 17, 21, 24, 30, 33, 38, 42, 45, 51, 52 };
	static u8 ring4_loc[15] = { 2, 4, 9, 13, 16, 22, 31, 32, 39, 41, 46, 50, 53, 59, 60 };
	static u8 ring5_loc[11] = { 1, 5, 8, 14, 23, 40, 47, 49, 54, 58, 61 };
	static u8 ring6_loc[10] = { 0, 6, 7, 15, 48, 55, 56, 57, 62, 63 };
	static u16 frame = 0;
	static constexpr u16 FRAME_TICKS = 100; // every second

	if (!init) {
		number = 9;
		frame = 0;
		init = true;
		rotation = utils_random_in_range(0, 3);
		// ring0[1]
	}

	if (number == -2) {
		// deinit
		init = false;
		state.phase = EXPLOSION;
		memset(buffer, 0, sizeof(buffer));
		buffer_transfer();
		state_set_minus(); // TODO - move to darkness...
		return;
	}

	// color test
	fill_ring_with_color(ring0_loc, ARRAY_SIZE(ring0_loc), COLOR_WHITE);
	fill_ring_with_color(ring1_loc, ARRAY_SIZE(ring1_loc), COLOR_RED);
	fill_ring_with_color(ring2_loc, ARRAY_SIZE(ring2_loc), COLOR_ORANGE);
	fill_ring_with_color(ring3_loc, ARRAY_SIZE(ring3_loc), COLOR_YELLOW);
	fill_ring_with_color(ring4_loc, ARRAY_SIZE(ring4_loc), COLOR_WHITE);
	fill_ring_with_color(ring5_loc, ARRAY_SIZE(ring5_loc), COLOR_CYAN);
	fill_ring_with_color(ring6_loc, ARRAY_SIZE(ring6_loc), COLOR_BLUE);

	if (frame % FRAME_TICKS == 0) {
	}

	frame = (frame + 1) % FRAME_TICKS;
	rotate_buffer_left(rotation);
	buffer_transfer();
}

void wsleds_animation(const u16 frame) {
	switch (state.phase) {
		case IDLE:
			anim_explosion();
			break;
		case COUNTDOWN:
			anim_countdown();
			break;
		default:
			break;
	}
}
