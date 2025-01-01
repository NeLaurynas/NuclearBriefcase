// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NUMBERS_H
#define NUMBERS_H

#include <stdint.h>

void numbers_init();

void numbers_display(uint8_t number1, uint8_t number2);

void numbers_ok(bool ok);

void numbers_generate_target();

void anim(uint8_t frame);

#endif //NUMBERS_H
