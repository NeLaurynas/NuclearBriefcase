// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "switches.h"

#include "state.h"
#include "utils.h"
#include "defines/config.h"
#include "shared_modules/mcp/mcp.h"

static constexpr u8 leds[] = { MOD_SWITCHES_LED3_1, MOD_SWITCHES_LED3_2, MOD_SWITCHES_LED3_3 };

u8 switches_get_position() {
	if (mcp_is_pin_low(MOD_SWITCHES_SWITCH3_TOP)) return 0;
	if (mcp_is_pin_low(MOD_SWITCHES_SWITCH3_BOTTOM)) return 2;
	return 1;
}

void switches_manage_leds() {
	for (auto i = 0; i < 3; i++) {
		//  don't touch animation
		if (i == state.switches.target_position && state.switches.target_position != state.switches.switch_position) continue;
		mcp_set_out(leds[i], i == state.switches.switch_position);
	}
}

void switches_generate_position() {
	auto new_position = utils_random_in_range(0, 2);
	if (new_position == state.switches.switch_position) {
		const bool inc = utils_random_in_range(0, 1) == 0;
		if (inc) new_position = (new_position + 1) % 3;
		else new_position = new_position == 0 ? 2 : new_position - 1;
	}

	state.switches.target_position = new_position;
}

void switches_init() {
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_SWITCH1, false);
	mcp_cfg_set_pull_up(MOD_SWITCHES_SWITCH1, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_SWITCH2, false);
	mcp_cfg_set_pull_up(MOD_SWITCHES_SWITCH2, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_SWITCH3_TOP, false);
	mcp_cfg_set_pull_up(MOD_SWITCHES_SWITCH3_TOP, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_SWITCH3_BOTTOM, false);
	mcp_cfg_set_pull_up(MOD_SWITCHES_SWITCH3_BOTTOM, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED1_G, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED1_R, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED2_G, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED2_R, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED3_1, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED3_2, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED3_3, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED3_R, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED3_G, true);

	state.switches.switch_position = switches_get_position();
	switches_generate_position();
}

void switches_animation() {
	static u32 frame = 0;
	static constexpr u32 blink_interval = 20;
	static bool light = true;

	if (state.switches.target_position != state.switches.switch_position) {
		mcp_set_out(leds[state.switches.target_position], light);
		frame = (frame + 1) % blink_interval;
		if (frame == 0) light = !light;
	} else {
		frame = 0;
		light = false;
	}
}
