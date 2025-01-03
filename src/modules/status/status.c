// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#include "status.h"

#include "defines/config.h"
#include "modules/mcp/mcp.h"

void status_init() {
	mcp_cfg_set_pin_out_mode(MOD_STAT_LED_NUMBERS, true);
}

void status_set_on(const uint8_t led, const bool on) {
	mcp_set_out(led, on);
}