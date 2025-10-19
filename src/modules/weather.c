#include <pebble.h>
#include "weather.h"

void draw_weather(Layer *layer, GContext *ctx, GRect location, GFont font_used, int temperature, const char *condition, bool use_fahrenheit) {
  char weather_text[32];
  
  // Display weather data based on condition
  // Note: Temperature is already converted to the correct unit by JavaScript
  if (strcmp(condition, "ERROR") == 0) {
    snprintf(weather_text, sizeof(weather_text), "ERROR");
  } else if (strcmp(condition, "NO_GPS") == 0) {
    snprintf(weather_text, sizeof(weather_text), "NO GPS");
  } else if (strcmp(condition, "LOADING") == 0) {
    snprintf(weather_text, sizeof(weather_text), "LOADING");
  } else if (temperature != 0 || strcmp(condition, "CLEAR") == 0) {
    // Show weather data (temperature 0 is valid for freezing conditions)
    // Temperature is pre-converted to correct unit (C or F) by JavaScript
    snprintf(weather_text, sizeof(weather_text), "%d° %s", temperature, condition);
  } else {
    snprintf(weather_text, sizeof(weather_text), "--");
  }
  
  graphics_draw_text(ctx, weather_text, font_used, location,
                    GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}