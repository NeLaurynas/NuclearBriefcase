// Copyright (C) 2025 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef STATUS_H
#define STATUS_H

#include <stdint.h>

#include "shared_config.h"

void status_init();

void status_set_on(u8 led, i8 on);

#endif //STATUS_H
