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
#include "modules/numbers/numbers.h"
#include "modules/piezo/piezo.h"
#include "modules/switches/switches.h"

static const u8 line_width = (u8)sqrt(MOD_WSLEDS_LED_COUNT);

static u32 reduce_brightness(const u32 reduction, const u32 color) {
	u16 r = (color >> 16) & 0b11111111;
	u16 g = (color >> 8) & 0b11111111;
	u16 b = color & 0b11111111;

	r = (r > reduction) ? (r - reduction) : 0;
	g = (g > reduction) ? (g - reduction) : 0;
	b = (b > reduction) ? (b - reduction) : 0;

	return (utils_min(r, 255) << 16) | (utils_min(g, 255) << 8) | utils_min(b, 255);
}

static u32 buffer_top[MOD_WSLEDS_LED_COUNT] = { 0 };

static inline void buffer_transfer() {
	dma_channel_transfer_from_buffer_now(MOD_WSLEDS_DMA_CH, buffer_top, MOD_WSLEDS_LED_COUNT);
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
	dma_channel_configure(MOD_WSLEDS_DMA_CH, &dma_c, &MOD_WSLEDS_PIO->txf[MOD_WSLEDS_SM], buffer_top, MOD_WSLEDS_LED_COUNT,
	                      false);
	sleep_ms(1);

	// get clock divider
	const auto clk_div = utils_calculate_pio_clk_div_ns(98);
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

static void rotate_buffer_left(const u8 times) {
	static u32 temp[MOD_WSLEDS_LED_COUNT] = { 0 };
	if (times == 0) return;

	for (auto t = 0; t < times; t++) {
		for (auto i = 0; i < 8; i++) for (auto j = 0; j < 8; j++) temp[(7 - j) * 8 + i] = buffer_top[i * 8 + j];

		for (auto i = 0; i < MOD_WSLEDS_LED_COUNT; i++) {
			buffer_top[i] = temp[i];
		}
	}
}

static void anim_target() {
	static bool init = false;
	static u8 target_dot = 0;
	static u8 x_line = 0;
	static u8 y_line = 0;
	static u8 x_steps = 0;
	static u8 y_steps = 0;
	static u8 current_step = 0;
	static u16 frame = 1;
	static u16 green_x_frame = 0;
	static constexpr u16 FRAME_TICKS = 975;
	static constexpr u16 FRAME_TICK_DIVIDER = 75;
	static u8 freeze_frames = 0;

	if (!init) {
		target_dot = get_led_from_lines(utils_random_in_range(1, 6), utils_random_in_range(1, 6));
		x_steps = get_line_x(target_dot);
		y_steps = get_line_y(target_dot);
		init = true;
	}

	memset(buffer_top, 0, sizeof(buffer_top));

	// render target dot
	buffer_top[target_dot] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.5f, 10), COLOR_CYAN);

	// render X
	for (auto i = 0; i < line_width; i++) {
		const u8 x_led = x_led_start(x_line) + i;
		const u8 y_led = y_line + i * line_width;
		if (x_line == get_line_x(target_dot) && y_line == get_line_y(target_dot)) {
			// on target - blink green
			current_state.wsleds.on_target = true;
			buffer_top[y_led] = buffer_top[x_led] = reduce_brightness(
				anim_color_reduction(TO_DIM, green_x_frame, FRAME_TICKS, 1.0f, 10),
				COLOR_GREEN);
		} else {
			// show red blink dot over red lines
			current_state.wsleds.on_target = false;
			buffer_top[x_led] = get_line_x(x_led) == get_line_x(target_dot) && get_line_y(x_led) == get_line_y(target_dot)
				&&
				buffer_top[target_dot] != 0
				? buffer_top[target_dot]
				: COLOR_RED;
			buffer_top[y_led] = get_line_x(y_led) == get_line_x(target_dot) && get_line_y(y_led) == get_line_y(target_dot)
				&&
				buffer_top[target_dot] != 0
				? buffer_top[target_dot]
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
				green_x_frame = 0;
			} else {
				freeze_frames--;
				goto end;
			}
			// calculate steps to take per frame for X
			x_steps = abs((i8)get_line_x(target_dot) - x_line);
			y_steps = abs((i8)get_line_y(target_dot) - y_line);
			current_step = 0;
			goto end; // transfers maybe reduced green
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
	if (x_line == get_line_x(target_dot) && y_line == get_line_y(target_dot))
		green_x_frame = (green_x_frame + 1) %
			FRAME_TICKS;
	buffer_transfer();
}

static void anim_countdown() {
	static bool init = false;
	static i8 number = 0;
	static u16 frame = 0;
	static constexpr u16 FRAME_TICKS = 100; // every second

	if (!init) {
		number = 9;
		frame = 0;
		init = true;
		memset(buffer_top, 0, sizeof(buffer_top));
	}

	if (number == -2) {
		// deinit
		init = false;
		state.phase = PHASE_EXPLOSION;
		piezo_play(MUSIC_UNDERWORLD);
		memset(buffer_top, 0, sizeof(buffer_top));
		buffer_transfer();
		return;
	}

	if (frame % FRAME_TICKS == 0) {
		if (number >= 0) {
			memcpy(buffer_top, WSLEDS_NUMBERS[number], sizeof(buffer_top));
			rotate_buffer_left(3);
			state.piezo.freq = 2700.f - (float)(number * 150);
			piezo_play(PIEZO_CUSTOM);
		}
		number--;
	}
	for (u8 i = 0; i < MOD_WSLEDS_LED_COUNT; i++) { // why use led count config, when a lot assumes it's 8x8...
		if (buffer_top[i] == 0) continue;

		u32 color = COLOR_RED;
		if (number + 1 >= 0 && number + 1 <= 2) {
			color = COLOR_GREEN;
		} else if (number + 1 >= 3 && number + 1 <= 5) {
			color = COLOR_YELLOW;
		}

		buffer_top[i] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.00f, 1), color);
	}

	if (number != -2) {
		buffer_transfer();
		frame = (frame + 1) % FRAME_TICKS;
	}
}

static inline void fill_ring_with_color(const u8 *ring, const u8 size, const u32 color) {
	for (auto i = 0; i < size; i++) {
		buffer_top[ring[i]] = color;
	}
}

static void anim_explosion() {
	static bool init = false;
	static u32 stage = 0;
	static u32 rotation = 0;
	static constexpr u8 ring0_loc[1] = { 27 };
	static constexpr u8 ring1_loc[5] = { 19, 26, 28, 35, 36 };
	static constexpr u8 ring2_loc[9] = { 11, 18, 20, 25, 29, 34, 37, 43, 44 };
	static constexpr u8 ring3_loc[13] = { 3, 10, 12, 17, 21, 24, 30, 33, 38, 42, 45, 51, 52 };
	static constexpr u8 ring4_loc[15] = { 2, 4, 9, 13, 16, 22, 31, 32, 39, 41, 46, 50, 53, 59, 60 };
	static constexpr u8 ring5_loc[11] = { 1, 5, 8, 14, 23, 40, 47, 49, 54, 58, 61 };
	static constexpr u8 ring6_loc[10] = { 0, 6, 7, 15, 48, 55, 56, 57, 62, 63 };
	static u8 const *rings[] = {
		ring0_loc,
		ring1_loc,
		ring2_loc,
		ring3_loc,
		ring4_loc,
		ring5_loc,
		ring6_loc
	};
	static constexpr u8 ring_sizes[] = {
		sizeof(ring0_loc) / sizeof(ring0_loc[0]),
		sizeof(ring1_loc) / sizeof(ring1_loc[0]),
		sizeof(ring2_loc) / sizeof(ring2_loc[0]),
		sizeof(ring3_loc) / sizeof(ring3_loc[0]),
		sizeof(ring4_loc) / sizeof(ring4_loc[0]),
		sizeof(ring5_loc) / sizeof(ring5_loc[0]),
		sizeof(ring6_loc) / sizeof(ring6_loc[0])
	};
	static u32 ring_colors[7] = { COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_WHITE, COLOR_YELLOW, COLOR_ORANGE,
	                              COLOR_RED };
	static u16 frame = 0;
	static constexpr u16 FRAME_TICKS = 33;

	if (!init) {
		stage = 0;
		frame = 1;
		init = true;
		rotation = utils_random_in_range(0, 3);
		memset(buffer_top, 0, sizeof(buffer_top));
	}

	if (stage == 14) {
		// deinit
		init = false;
		memset(buffer_top, 0, sizeof(buffer_top));
		buffer_transfer();
		state.phase = PHASE_DARKNESS;
		return;
	}

	for (u32 i = 0; i < ARRAY_SIZE(rings); i++) {
		const i32 x = (i32)(stage - i);
		const auto ring = rings[i];
		const auto size = ring_sizes[i];
		if (x < 0 || x > 7) fill_ring_with_color(ring, size, COLOR_OFF);
		else if (x == 0) {
			fill_ring_with_color(ring, size,
			                     reduce_brightness(anim_color_reduction(TO_BRIGHT, frame - 1, FRAME_TICKS, 1.0f, 1.0f),
			                                       COLOR_RED));
		} else if (x < 7) {
			const auto color = anim_color_blend(ring_colors[x - 1], ring_colors[x], frame - 1, FRAME_TICKS, 1.0f, 1.0f);
			fill_ring_with_color(ring, size, color);
		} else {
			const auto color = reduce_brightness(anim_color_reduction(TO_DIM, frame - 1, FRAME_TICKS, 1.0f, 1.0f),
			                                     COLOR_RED);
			fill_ring_with_color(ring, size, color);
		}
	}

	// random flicker
	if (frame % 10 == 0)
		for (auto i = 0; i < MOD_WSLEDS_LED_COUNT; i++) {
			if (buffer_top[i] == 0) continue;
			const u32 reduction = utils_random_in_range(0, 8) - 4;
			buffer_top[i] = reduce_brightness(reduction, buffer_top[i]);
		}

	if (frame % FRAME_TICKS == 0) stage++;

	if (stage != 14) {
		frame = (frame + 1);
		rotate_buffer_left(rotation);
		buffer_transfer();
	}
}

static void anim_darkness() {
	static bool init = false;
	static u32 cycle = 0;
	static u32 frame = 0;
	static u32 cycles = 6;
	static constexpr u32 FRAME_TICKS = 64;

	if (!init) {
		cycle = 0;
		frame = 1;
		init = true;
		// dependencies from other modules, not great
		state_set_minus();
		numbers_generate_target();
		switches_generate_position();
	}

	if (cycle == cycles + 3) {
		// deinit
		init = false;
		state.phase = PHASE_IDLE;
		return;
	}

	// random flicker
	if (frame % 10 == 0)
		for (auto i = 0; i < MOD_WSLEDS_LED_COUNT; i++) {
			if (buffer_top[i] == 0) continue;
			const u32 reduction = utils_random_in_range(0, 8) - 4;
			buffer_top[i] = reduce_brightness(reduction, buffer_top[i]); // mm I dunno
		}

	if (frame % FRAME_TICKS == 0) {
		if (cycle <= cycles) {
			memset(buffer_top, 0, sizeof(buffer_top));
			const u32 color = current_state.wsleds.on_target ? COLOR_GREEN : COLOR_RED;
			for (u32 i = 0; i < utils_random_in_range(1, MOD_WSLEDS_LED_COUNT); i++) {
				buffer_top[utils_random_in_range(0, (MOD_WSLEDS_LED_COUNT - 1))] =
					reduce_brightness(255 - ((255 / cycles) * cycle), color); // really?
			}
		}

		cycle++;
	}

	frame++;
	buffer_transfer();
}

static void anim_error(void) {
	static bool init = false;
	static u32 cycle = 0;
	static u32 frame = 0;
	static u32 cycles = 3;
	static constexpr u16 FRAME_TICKS = 50;

	if (!init) {
		cycle = 0;
		frame = 0;
		init = true;
	}

	if (frame % FRAME_TICKS == 0) {
		memcpy(buffer_top, WSLEDS_ERROR, sizeof(buffer_top));
		cycle++;
		if (cycle != cycles) piezo_play(PIEZO_SHORT_ERROR);
	}
	for (auto i = 0; i < MOD_WSLEDS_LED_COUNT; i++) {
		if (buffer_top[i] == 0) continue;
		buffer_top[i] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.00f, 1), COLOR_RED);
	}

	if (cycle == cycles) {
		// deinit
		init = false;
		state.phase = PHASE_IDLE;
	} else {
		frame++;
		buffer_transfer();
	}
}

void wsleds_animation() {
	switch (state.phase) {
		case PHASE_IDLE:
			anim_target();
			break;
		case PHASE_COUNTDOWN:
			anim_countdown();
			break;
		case PHASE_EXPLOSION:
			anim_explosion();
			break;
		case PHASE_DARKNESS:
			anim_darkness();
			break;
		case PHASE_ERROR:
			anim_error();
			break;
		default:
			break;
	}
}
