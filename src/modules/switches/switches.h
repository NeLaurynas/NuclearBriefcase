// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef SWITCHES_H
#define SWITCHES_H
#include <shared_config.h>

u8 switches_get_position();

void switches_manage_leds();

void switches_generate_position();

void switches_init();

void switches_animation();

#endif //SWITCHES_H
