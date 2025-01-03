// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STATUS_H
#define STATUS_H

#include <stdint.h>

#include "defines/config.h"

void status_init();

void status_set_on(u8 led, bool on);

#endif //STATUS_H
