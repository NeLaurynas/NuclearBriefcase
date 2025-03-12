// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "status.h"

#include "defines/config.h"
#include "shared_modules/mcp/mcp.h"

void status_init() {
	// mcp_cfg_set_pin_out_mode(MOD_STAT_LED_NUMBERS, true);
}

// todo: refactor - have a list of leds, and this increments the counter and lights the required amount of LEDs
void status_set_on(const u8 led, const i8 on) {
	// mcp_set_out(led, on > 0);
}
