#include <pebble.h>
#include <ctype.h>
#include "main.h"
#include "modules/date.h"
#include "modules/steps.h"
#include "modules/number.h"

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
  settings.Setting24H = true;
  settings.SettingShowAMPM = false;
  snprintf(settings.TopModule, sizeof(settings.TopModule), "date");
  snprintf(settings.BottomModule, sizeof(settings.BottomModule), "steps");
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
  // Background color
  window_set_background_color(s_main_window, settings.BackgroundColor);
  layer_set_update_proc(s_canvas_layer, draw_watchface);
}

// Function to update step count
static void update_steps() {
  s_step_count = health_service_sum_today(HealthMetricStepCount);
}

static void draw_time(Layer *layer, GContext *ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  // Get the current hour and minute
  int hour = t->tm_hour;
      
  // Convert to 12-hour format if IS_24 is false
  if (settings.Setting24H == false) {
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }
  
  int minute = t->tm_min;

  // Get the bounds of the canvas
  GRect bounds = layer_get_bounds(layer);

  // get first number from the hour
  int first_hour_digit = hour < 10 ? 0 : hour / 10;
  // get second number from the hour
  int second_hour_digit = hour % 10;

  int hight_of_block = (bounds.size.h / 2) - 40;

  GRect first_hour_bound = GRect(0, 35, bounds.size.w / 2, hight_of_block);
  draw_single_digit(layer, ctx, first_hour_bound, hight_of_block, first_hour_digit, 1, settings.ForegroundColor);

  GRect second_hour_bound = GRect(bounds.size.w / 2, 35, bounds.size.w / 2, hight_of_block);
  draw_single_digit(layer, ctx, second_hour_bound, hight_of_block, second_hour_digit, 0, settings.ForegroundColor);

  // get first number from the minute
  int first_minute_digit = minute < 10 ? 0 : minute / 10;
  // get second number from the minute
  int second_minute_digit = minute % 10;

  GRect first_minute_bound = GRect(0, 35 + hight_of_block + 10, bounds.size.w / 2, hight_of_block);
  draw_single_digit(layer, ctx, first_minute_bound, hight_of_block, first_minute_digit, 1, settings.ForegroundColor);
  GRect second_minute_bound = GRect(bounds.size.w / 2, 35 + hight_of_block + 10, bounds.size.w / 2, hight_of_block);
  draw_single_digit(layer, ctx, second_minute_bound, hight_of_block, second_minute_digit, 0, settings.ForegroundColor);

  // Draw AM/PM if 12-hour format is selected
  if (settings.Setting24H == false && settings.SettingShowAMPM) {
    // Determine if it's AM or PM based on the original hour
    bool is_pm = t->tm_hour >= 12;
    char ampm_text[3];
    
    if (is_pm) {
        strcpy(ampm_text, "PM");
    } else {
        strcpy(ampm_text, "AM");
    }

    GSize ampm_size = graphics_text_layout_get_content_size(ampm_text, s_date_font,  bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft);
    
    int ampm_x = (bounds.size.w / 2) + ((hight_of_block / 7) * 5) + 10; 
    // Align bottom of AM/PM with bottom of minutes
    int ampm_y = (hight_of_block * 2) + 40 - (ampm_size.h);
    
    GRect ampm_bounds = GRect(ampm_x, ampm_y, 30, 20);
    graphics_draw_text(ctx, ampm_text, s_date_font, ampm_bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
  }
}

static void draw_watchface(Layer *layer, GContext *ctx) {

  // Get the bounds of the canvas
  GRect bounds = layer_get_bounds(layer);

  graphics_context_set_text_color(ctx, settings.ForegroundColor);

  GRect top_bounds = GRect(0, 10, bounds.size.w, 20);
  GRect low_bounds = GRect(0, bounds.size.h - 30, bounds.size.w, 20);
  
  //top module
  if (strcmp(settings.TopModule, "steps") == 0) draw_steps(layer, ctx, top_bounds, s_date_font, s_step_count);
  if (strcmp(settings.TopModule, "date") == 0) draw_date(layer, ctx, top_bounds, s_date_font);
  // center module
  draw_time(layer, ctx);
  //bottom module
  if (strcmp(settings.BottomModule, "steps") == 0) draw_steps(layer, ctx, low_bounds, s_date_font, s_step_count);
  if (strcmp(settings.BottomModule, "date") == 0) draw_date(layer, ctx, low_bounds, s_date_font);

}

static void main_window_load(Window *window) {
  // Create canvas layer
  s_canvas_layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
  layer_set_update_proc(s_canvas_layer, draw_watchface);
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  
  // Load fonts
  s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_TINY_FONT_16));

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
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Deinitialize custom fonts
  fonts_unload_custom_font(s_date_font);

  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}