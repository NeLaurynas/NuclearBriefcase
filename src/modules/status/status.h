// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STATUS_H
#define STATUS_H

#include <stdint.h>

void status_init();

void status_set_on(uint8_t led, bool on);

#endif //STATUS_H
