// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NUMBERS_H
#define NUMBERS_H

#include "defines/config.h"

void numbers_init();

void numbers_display(u8 number1, u8 number2);

void numbers_ok(bool ok);

void numbers_generate_target();

void numbers_inc();

void numbers_dec();

void anim(u8 frame);

#endif //NUMBERS_H
