// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "status.h"

#include "state.h"
#include "defines/config.h"
#include "shared_modules/mcp/mcp.h"

static constexpr u8 leds[] = { MOD_STAT_LED_1, MOD_STAT_LED_2, MOD_STAT_LED_3, MOD_STAT_LED_4 };

void status_init() {
	mcp_cfg_set_pin_out_mode(MOD_STAT_LED_1, true);
	mcp_cfg_set_pin_out_mode(MOD_STAT_LED_2, true);
	mcp_cfg_set_pin_out_mode(MOD_STAT_LED_3, true);
	mcp_cfg_set_pin_out_mode(MOD_STAT_LED_4, true);
}

void status_render_leds() {
	auto ok_statuses = 0;
	if (state_get_bool(state.status.numbers_on)) ok_statuses++;
	if (state_get_bool(state.status.switches1_on)) ok_statuses++;
	if (state_get_bool(state.status.switches2_on)) ok_statuses++;
	if (state_get_bool(state.status.switches3_on)) ok_statuses++;

	for (auto i = 0; i < ok_statuses; i++) mcp_set_out(leds[i], true);
	for (auto i = 3; i >= ok_statuses; i--) mcp_set_out(leds[i], false);
}
