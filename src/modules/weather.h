#pragma once
#include <pebble.h>

void draw_weather(Layer *layer, GContext *ctx, GRect location, GFont font_used, int temperature, const char *condition, bool use_fahrenheit);