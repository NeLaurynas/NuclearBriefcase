// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef MPC_H
#define MPC_H
#include <stdint.h>

void mcp_init();

void mcp_cfg_set_pin_out_mode(uint8_t data, bool is_out);

void mcp_cfg_set_pull_up(uint8_t pinData, bool pull_up);

void mcp_set_out(uint8_t pinData, bool out);

bool mcp_is_pin_low(uint8_t pinData);

void mcp_all();

#endif //MPC_H
