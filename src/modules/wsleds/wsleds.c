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
#include "utils.h"
#include "defines/config.h"

static constexpr u32 c_yellow = 0b11111111'11111111'00000000;
static constexpr u32 c_green = 0b00000000'11111111'00000000;
static constexpr u32 c_red = 0b11111111'00000000'00000000;
static constexpr u32 c_white = 0b11111111'11111111'11111111;
static constexpr u32 c_blue = 0b00000000'00000000'11111111;
static constexpr u32 c_purple = 0b11111111'00000000'11111111;
static constexpr u32 c_cyan = 0b00000000'11111111'11111111;
static constexpr u32 c_off = 0b00000000'00000000'00000000;

static u8 line_width = (u8)sqrt(MOD_WSLEDS_LED_COUNT);

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

void wsleds_init() {
	// init flag (which is hardcoded for 64 leds kek)
	// @formatter:off
	// auto border_color = c_off;
	//
	// const u32 data[MOD_WSLEDS_LED_COUNT] = {
	// 	border_color, border_color, border_color, border_color, border_color, border_color, border_color, border_color,
	// 	c_yellow, c_yellow, c_yellow, c_yellow, c_yellow, c_yellow,	c_yellow, c_yellow,
	// 	c_yellow, c_yellow, c_yellow,	c_yellow, c_yellow, c_yellow,	c_yellow, c_yellow,
	// 	c_green, c_green, c_green, c_green,	c_green, c_green, c_green, c_green,
	// 	c_green, c_green, c_green, c_green,	c_green, c_green, c_green, c_green,
	// 	c_red, c_red, c_red, c_red,	c_red, c_red, c_red, c_red,
	// 	c_red, c_red, c_red, c_red,	c_red, c_red, c_red, c_red,
	// 	border_color, border_color, border_color, border_color, border_color, border_color, border_color, border_color,
	// };
	// @formatter:on
	// memcpy(buffer, data, sizeof(data));

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
	const auto clk_div = utils_calculate_pio_clk_div_ns(88);
	utils_printf("WSLEDS PIO CLK DIV: %f\n", clk_div);

	// init PIO
	const auto offset = pio_add_program(MOD_WSLEDS_PIO, &pio_wsleds_program);
	if (offset < 0) utils_error_mode(26);
	if (pio_sm_is_claimed(MOD_WSLEDS_PIO, MOD_WSLEDS_SM)) utils_error_mode(27);
	pio_sm_claim(MOD_WSLEDS_PIO, MOD_WSLEDS_SM);
	pio_wsleds_program_init(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, offset, MOD_WSLEDS_PIN, clk_div);
	pio_sm_set_enabled(MOD_WSLEDS_PIO, MOD_WSLEDS_SM, true);
	sleep_ms(1);

	wsleds_transfer(); // show flag idk
}

void wsleds_transfer() {
	dma_channel_transfer_from_buffer_now(MOD_WSLEDS_DMA_CH, buffer, MOD_WSLEDS_LED_COUNT);
}

void wsleds_anim_flag() {
	static u8 brightness = 0;
	for (u8 i = 8; i < 56; i++) {
		switch (i) {
		case 8 ... 23: buffer[i] = reduce_brightness(brightness + i - i / 2, c_yellow);
			break;

		case 24 ... 39: buffer[i] = reduce_brightness(brightness + i - i / 2, c_green);
			break;

		case 40 ... 55: buffer[i] = reduce_brightness(brightness + i - i / 2, c_red);
			break;

		default: break;
		}
	}
	wsleds_transfer();
	brightness = brightness + 1;
}

inline u8 x_led_start(u8 x_line) {
	return x_line * line_width;
}

inline u8 get_line_x(u8 dot) {
	return dot / line_width;
}

inline u8 get_line_y(u8 dot) {
	return dot % line_width;
}

static u8 get_led_from_lines(const u8 x_line, const u8 y_line) {
	return line_width * y_line + x_line;
}

void wsleds_anim_target() {
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
	static float x_steps_per_step = 0;
	static float y_steps_per_step = 0;

	if (!init) {
		target_dot = get_led_from_lines(utils_random_in_range(1, 6), utils_random_in_range(1, 6));
		x_steps = get_line_x(target_dot);
		y_steps = get_line_y(target_dot);
		init = true;
	}

	memset(buffer, 0, sizeof(buffer));

	// render target dot
	buffer[target_dot] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.5f, 10), c_blue);

	// render X
	for (u8 i = 0; i < line_width; i++) {
		const u8 x_led = x_led_start(x_line) + i;
		const u8 y_led = y_line + i * line_width;
		if (x_line == get_line_x(target_dot) && y_line == get_line_y(target_dot)) {
			// on target - blink green
			buffer[x_led] = reduce_brightness(anim_color_reduction(TO_DIM, frame, FRAME_TICKS, 1.0f, 10), c_green);
			buffer[y_led] = buffer[x_led];
		} else {
			// blink dot over red lines
			buffer[x_led] = get_line_x(x_led) == get_line_x(target_dot) && get_line_y(x_led) == get_line_y(target_dot) &&
				buffer[target_dot] != 0
				? buffer[target_dot]
				: c_red;
			buffer[y_led] = get_line_x(y_led) == get_line_x(target_dot) && get_line_y(y_led) == get_line_y(target_dot) &&
				buffer[target_dot] != 0
				? buffer[target_dot]
				: c_red;
		}
	}

	if (frame % FRAME_TICK_DIVIDER == 0) {
		if (get_line_x(target_dot) == x_line && get_line_y(target_dot) == y_line) {
			if (freeze_frames == 0) {
				freeze_frames = 5;
			}
			if (freeze_frames == 1) {
				// target_dot = get_led_from_lines(utils_random_in_range(1, 6), utils_random_in_range(1, 6));
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
	wsleds_transfer();
}
