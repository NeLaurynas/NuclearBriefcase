// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "switches.h"

#include "defines/config.h"
#include "shared_modules/mcp/mcp.h"

void switches_set_on(const u8 led, const u8 on) {
	mcp_set_out(led, on > 0);
}

void switches_init() {
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_SWITCH1, false);
	mcp_cfg_set_pull_up(MOD_SWITCHES_SWITCH1, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_SWITCH2, false);
	mcp_cfg_set_pull_up(MOD_SWITCHES_SWITCH2, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED1_G, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED1_R, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED2_G, true);
	mcp_cfg_set_pin_out_mode(MOD_SWITCHES_LED2_R, true);
}
