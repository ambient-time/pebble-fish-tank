// Copyright 2026 Luke Steuber. MIT License.
#pragma once
#include <stdint.h>
#include <stdbool.h>
typedef struct {int32_t revision,enabled,at;char series[99];} Tide;
bool tide_valid(const Tide *t);
int tide_level(const Tide *t,int64_t now);
int tide_surface(int level,bool round);
