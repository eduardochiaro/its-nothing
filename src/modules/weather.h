#pragma once
#include <pebble.h>

#pragma once

void draw_weather(Layer *layer, GContext *ctx, GRect location, GFont font_used, int temperature_celsius, const char *condition, bool use_fahrenheit);