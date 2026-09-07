// Copyright 2026 Luke Steuber. MIT License.
#pragma once
#include <stdbool.h>
#include <stdint.h>
typedef struct { int hour, tens, ones, bubbles; bool pm, daylight; } TankTime;
typedef struct { int x,y,direction,kind; } TankFish;
TankTime tank_time(int hour,int minute,int second);
TankFish tank_fish(int index,int count,int width,int height,bool round,uint32_t eighths);
uint32_t tank_hash(uint32_t value);
