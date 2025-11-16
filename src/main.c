#include <pebble.h>
#include <ctype.h>
#include "main.h"
#include "modules/date.h"
#include "modules/steps.h"
#include "modules/number.h"
#include "modules/weather.h"

static Window *s_main_window;
static Layer *s_canvas_layer;
static GFont s_date_font;
static int s_step_count = 0;

// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.ForegroundColor = GColorWhite;
  settings.Setting24H = false;
  settings.SettingShowAMPM = true;
  settings.SettingShowSeconds = false;
  settings.DotThickness = false;
  snprintf(settings.TopModule, sizeof(settings.TopModule), "date");
  snprintf(settings.BottomModule, sizeof(settings.BottomModule), "weather");
  settings.WeatherTemperature = 0;
  snprintf(settings.WeatherCondition, sizeof(settings.WeatherCondition), "--");
  settings.WeatherUseFahrenheit = false;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
  layer_mark_dirty(s_canvas_layer);
  window_set_background_color(s_main_window, settings.BackgroundColor);
}

// Function to update step count
static void update_steps() {
  s_step_count = health_service_sum_today(HealthMetricStepCount);
}

// Helper function to draw a module at specified bounds
static void draw_module(Layer *layer, GContext *ctx, GRect bounds, const char *module_name) {
  if (strcmp(module_name, "steps") == 0) draw_steps(layer, ctx, bounds, s_date_font, s_step_count);
  else if (strcmp(module_name, "date") == 0) draw_date(layer, ctx, bounds, s_date_font);
  else if (strcmp(module_name, "weather") == 0) draw_weather(layer, ctx, bounds, s_date_font, settings.WeatherTemperature, settings.WeatherCondition, settings.WeatherUseFahrenheit);
}

static void draw_time(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // Get the current hour and minute
  int hour = t->tm_hour;
      
  // Convert to 12-hour format if not 24H
  if (!settings.Setting24H) {
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }

  int dot_size = settings.DotThickness ? 1 : 0;
  int minute = t->tm_min;

  // Get the bounds of the canvas
  GRect bounds = layer_get_bounds(layer);

  // Calculate digits
  int first_hour_digit = hour / 10;
  int second_hour_digit = hour % 10;
  int first_minute_digit = minute / 10;
  int second_minute_digit = minute % 10;

  int height_of_block = (bounds.size.h / 2) - 40 - dot_size;
  int y_offset = TIME_TOP_MARGIN - (dot_size * 2);

  // Draw hour digits
  GRect first_hour_bound = GRect(0, y_offset, bounds.size.w / 2, height_of_block);
  draw_single_digit(layer, ctx, first_hour_bound, height_of_block, first_hour_digit, 1, settings.ForegroundColor, dot_size);

  GRect second_hour_bound = GRect(bounds.size.w / 2, y_offset, bounds.size.w / 2, height_of_block);
  draw_single_digit(layer, ctx, second_hour_bound, height_of_block, second_hour_digit, 0, settings.ForegroundColor, dot_size);

  // Draw minute digits
  int minute_y_offset = y_offset + height_of_block + TIME_CENTER_SPACING;
  GRect first_minute_bound = GRect(0, minute_y_offset, bounds.size.w / 2, height_of_block);
  draw_single_digit(layer, ctx, first_minute_bound, height_of_block, first_minute_digit, 1, settings.ForegroundColor, dot_size);
  
  GRect second_minute_bound = GRect(bounds.size.w / 2, minute_y_offset, bounds.size.w / 2, height_of_block);
  draw_single_digit(layer, ctx, second_minute_bound, height_of_block, second_minute_digit, 0, settings.ForegroundColor, dot_size);

  // Draw AM/PM if 12-hour format is selected
  if (!settings.Setting24H && settings.SettingShowAMPM) {
    const char *ampm_text = (t->tm_hour >= 12) ? "PM" : "AM";
    
    GSize ampm_size = graphics_text_layout_get_content_size(ampm_text, s_date_font, bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft);
    
    int top_padding = settings.DotThickness ? 0 : 8;
    int left_padding = settings.DotThickness ? 5 : 0;
    
    #if defined(PBL_PLATFORM_CHALK)
      if (settings.DotThickness) {
        top_padding = 10;
      }
    #endif

    int ampm_x = (bounds.size.w / 2) + ((height_of_block / 7) * 5) + 10 + left_padding; 
    int ampm_y = bounds.size.h - BOTTOM_MARGIN - ampm_size.h - top_padding;
    
    GRect ampm_bounds = GRect(ampm_x, ampm_y, AMPM_WIDTH, AMPM_HEIGHT);
    graphics_draw_text(ctx, ampm_text, s_date_font, ampm_bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  }

  // Draw seconds if enabled
  if (settings.SettingShowSeconds) {
    char seconds_text[3];
    snprintf(seconds_text, sizeof(seconds_text), "%02d", t->tm_sec);
    
    GSize seconds_size = graphics_text_layout_get_content_size(seconds_text, s_date_font, bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft);
    
    int top_padding = settings.DotThickness ? 10 : 18;
    int left_padding = settings.DotThickness ? 5 : 0;
    
    #if defined(PBL_PLATFORM_CHALK)
      if (settings.DotThickness) {
        top_padding = 20;
      }
    #endif

    int seconds_x = (bounds.size.w / 2) + ((height_of_block / 7) * 5) + 10 + left_padding; 
    int seconds_y = y_offset - top_padding + 5;
    
    GRect seconds_bounds = GRect(seconds_x, seconds_y, AMPM_WIDTH, AMPM_HEIGHT);
    graphics_draw_text(ctx, seconds_text, s_date_font, seconds_bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  }
}

static void draw_watchface(Layer *layer, GContext *ctx) {
  // Get the bounds of the canvas
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_text_color(ctx, settings.ForegroundColor);

  GRect top_bounds = GRect(0, 3, bounds.size.w, MODULE_HEIGHT);
  GRect bottom_bounds = GRect(0, bounds.size.h - BOTTOM_MARGIN, bounds.size.w, MODULE_HEIGHT);
  
  // Top module
  draw_module(layer, ctx, top_bounds, settings.TopModule);
  
  // Center module (time)
  draw_time(layer, ctx);
  
  // Bottom module
  draw_module(layer, ctx, bottom_bounds, settings.BottomModule);
}

static void main_window_load(Window *window) {
  // Create canvas layer
  s_canvas_layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_set_update_proc(s_canvas_layer, draw_watchface);
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  
  // Load fonts with error handling
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_PIXEL_FONT_20));
  if (!s_date_font) {
    // Fallback to system font if custom font fails to load
    s_date_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  }

  prv_update_display();
}

static void main_window_unload(Window *window) {
  layer_destroy(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(s_canvas_layer);
}

static void health_handler(HealthEventType event, void *context) {
  if (event == HealthEventSignificantUpdate || 
    event == HealthEventMovementUpdate) {
    update_steps();
    layer_mark_dirty(s_canvas_layer);
  }
}

static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
  // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
  }

  // Foreground Color
  Tuple *fg_color_t = dict_find(iter, MESSAGE_KEY_ForegroundColor);
  if (fg_color_t) {
    settings.ForegroundColor = GColorFromHEX(fg_color_t->value->int32);
  }

  // 12/24 Hour Format
  Tuple *setting_24h_t = dict_find(iter, MESSAGE_KEY_Setting24H);
  if(setting_24h_t) {
    settings.Setting24H = setting_24h_t->value->int32 == 1;
  }

  // AM/PM show/hide
  Tuple *setting_ampm_t = dict_find(iter, MESSAGE_KEY_SettingShowAMPM);
  if(setting_ampm_t) {
    settings.SettingShowAMPM = setting_ampm_t->value->int32 == 1;
  }

  // Seconds show/hide
  Tuple *setting_seconds_t = dict_find(iter, MESSAGE_KEY_SettingShowSeconds);
  if(setting_seconds_t) {
    settings.SettingShowSeconds = setting_seconds_t->value->int32 == 1;
    // Update tick timer subscription based on seconds setting
    tick_timer_service_unsubscribe();
    if (settings.SettingShowSeconds) {
      tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    } else {
      tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
    }
  }

  // Dot Thickness
  Tuple *dot_thickness_t = dict_find(iter, MESSAGE_KEY_DotThickness);
  if(dot_thickness_t) {
    settings.DotThickness = dot_thickness_t->value->int32 == 1;
  }

  // Top Module
  Tuple *top_module_t = dict_find(iter, MESSAGE_KEY_TopModule);
  if (top_module_t) {
    snprintf(settings.TopModule, sizeof(settings.TopModule), "%s", top_module_t->value->cstring);
  }

  // Bottom Module
  Tuple *bottom_module_t = dict_find(iter, MESSAGE_KEY_BottomModule);
  if (bottom_module_t) {
    snprintf(settings.BottomModule, sizeof(settings.BottomModule), "%s", bottom_module_t->value->cstring);
  }

  // Weather Temperature
  Tuple *weather_temp_t = dict_find(iter, MESSAGE_KEY_WeatherTemperature);
  if (weather_temp_t) {
    settings.WeatherTemperature = weather_temp_t->value->int32;  // Temperature is always in Celsius from JavaScript
  }

  // Weather Condition
  Tuple *weather_condition_t = dict_find(iter, MESSAGE_KEY_WeatherCondition);
  if (weather_condition_t) {
    snprintf(settings.WeatherCondition, sizeof(settings.WeatherCondition), "%s", weather_condition_t->value->cstring);
  }

  // Weather Use Fahrenheit
  Tuple *weather_fahrenheit_t = dict_find(iter, MESSAGE_KEY_WeatherUseFahrenheit);
  if (weather_fahrenheit_t) {
    settings.WeatherUseFahrenheit = weather_fahrenheit_t->value->int32 == 1;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

static void init() {
  prv_load_settings();
  s_main_window = window_create();
  
  window_set_background_color(s_main_window, settings.BackgroundColor);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });

  // Open AppMessage connection
  // Move in module later
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
  
  // Initialize health service
  health_service_events_subscribe(health_handler, NULL);
  
  // Get initial step count
  update_steps();
  
  window_stack_push(s_main_window, true);
  // Subscribe to appropriate time units based on seconds setting
  if (settings.SettingShowSeconds) {
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
  } else {
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  }
}

static void deinit() {
  // Deinitialize custom fonts
  if (s_date_font && s_date_font != fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)) {
    fonts_unload_custom_font(s_date_font);
  }

  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}