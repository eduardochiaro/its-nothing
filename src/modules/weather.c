#include <pebble.h>
#include "weather.h"

// Convert Celsius to Fahrenheit
static int celsius_to_fahrenheit(int celsius) {
  return (celsius * 9 / 5) + 32;
}

void draw_weather(Layer *layer, GContext *ctx, GRect location, GFont font_used, int temperature_celsius, const char *condition, bool use_fahrenheit) {
  char weather_text[32];
  
  // Display weather data based on condition
  if (strcmp(condition, "ERROR") == 0) {
    snprintf(weather_text, sizeof(weather_text), "ERROR");
  } else if (strcmp(condition, "NO_GPS") == 0) {
    snprintf(weather_text, sizeof(weather_text), "NO GPS");
  } else if (strcmp(condition, "LOADING") == 0) {
    snprintf(weather_text, sizeof(weather_text), "LOADING");
  } else if (temperature_celsius != 0 || strcmp(condition, "CLEAR") == 0) {
    // Convert temperature if needed (input is always Celsius from JavaScript)
    int display_temperature = use_fahrenheit ? 
      celsius_to_fahrenheit(temperature_celsius) : 
      temperature_celsius;
    
    // Show unit indicator
    const char *unit = use_fahrenheit ? "F" : "C";
    
    // Show weather data (temperature 0 is valid for freezing conditions)
    snprintf(weather_text, sizeof(weather_text), "%d°%s %s", display_temperature, unit, condition);
  } else {
    snprintf(weather_text, sizeof(weather_text), "--");
  }
  
  graphics_draw_text(ctx, weather_text, font_used, location,
                    GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}