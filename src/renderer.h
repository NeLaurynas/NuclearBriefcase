// Copyright (C) 2024 Laurynas 'Deviltry' Ekekeke
// SPDX-License-Identifier: BSD-3-Clause

#ifndef RENDERER_H
#define RENDERER_H

#include "shared_config.h"

void renderer_init(void (*animation_functions[])(), u8 animation_function_count);

void renderer_loop();

#endif //RENDERER_H
