// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef MPC_H
#define MPC_H
#include <stdint.h>

void mcp_init();
void mcp_set_out(uint8_t pinData, bool out);
void mcp_all();

#endif //MPC_H
