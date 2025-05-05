#pragma once
#include <pebble.h>


#define SETTINGS_KEY 1

// A structure containing our settings
typedef struct ClaySettings {
  GColor BackgroundColor;
  GColor ForegroundColor;
  bool Setting24H;
  bool SettingShowAMPM;
} __attribute__((__packed__)) ClaySettings;

// Function prototypes
static void prv_default_settings();
static void prv_load_settings();
static void prv_save_settings();
static void prv_update_display();
static void update_steps();
static void draw_time(Layer *layer, GContext *ctx);
static void draw_watchface(Layer *layer, GContext *ctx);
static void main_window_load(Window *window);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void health_handler(HealthEventType event, void *context);
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context);
static void init();
static void deinit();