#include <pebble.h>
#include <ctype.h>

static Window *s_main_window;
static Layer *s_canvas_layer;
static GFont s_time_font;
static GFont s_date_font;
static GFont s_ampm_font;
static int s_step_count = 0;

#define IS_24 false

// Function to update step count
static void update_steps() {
    s_step_count = health_service_sum_today(HealthMetricStepCount);
}

static void draw_watchface(Layer *layer, GContext *ctx) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Get the current hour and minute
    // Get the current hour and minute
    int hour = t->tm_hour;
        
    // Convert to 12-hour format if IS_24 is false
    if (IS_24 == false) {
        hour = hour % 12;
        if (hour == 0) hour = 12; // 12 instead of 0 for 12-hour format
    }
   
   int minute = t->tm_min;

    // Get the bounds of the canvas
    GRect bounds = layer_get_bounds(layer);
    
    // Set drawing context - changed to white text
    graphics_context_set_text_color(ctx, GColorWhite);
    
    // Get weekday and day
    char weekday_buffer[16];
    strftime(weekday_buffer, sizeof(weekday_buffer), "%a", t);
    
    // Convert weekday to uppercase
    for(int i = 0; weekday_buffer[i]; i++) {
        weekday_buffer[i] = toupper((unsigned char)weekday_buffer[i]);
    }
    
    char day_buffer[8];
    snprintf(day_buffer, sizeof(day_buffer), "%d", t->tm_mday);
    
    // Create date string (weekday + day)
    char date_text[24];
    snprintf(date_text, sizeof(date_text), "%s %s", weekday_buffer, day_buffer);
    
    // Draw date at the top
    int date_height = 20; // Approximate height needed for date text
    GRect date_bounds = GRect(0, 5, bounds.size.w, date_height);
    graphics_draw_text(ctx, date_text, s_date_font, date_bounds,
                      GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    
    // Create strings for hour and minute
    char hour_text[8];
    snprintf(hour_text, sizeof(hour_text), "%02d", hour);
    
    char minute_text[8];
    snprintf(minute_text, sizeof(minute_text), "%02d", minute);
    
    // Calculate text sizes
    GSize hour_text_size = graphics_text_layout_get_content_size(hour_text, s_time_font, 
                         bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
    
    GSize minute_text_size = graphics_text_layout_get_content_size(minute_text, s_time_font, 
                           bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
    
    // Calculate vertical spacing and positioning
    int vertical_spacing = 1; // Space between hour and minute
    int total_time_height = hour_text_size.h + minute_text_size.h + vertical_spacing;
    
    // Calculate the starting y position to center the time in the available space
    // (accounting for the date area at top)
    int available_height = bounds.size.h - date_height - 15; // 15px for padding
    int time_start_y = date_height - 10 + (available_height - total_time_height) / 2;
    
    // Draw hour (centered horizontally)
    GRect hour_bounds = GRect(0, time_start_y, bounds.size.w, hour_text_size.h);
    graphics_draw_text(ctx, hour_text, s_time_font, hour_bounds,
                      GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    
    // Draw minute below hour (centered horizontally)
    GRect minute_bounds = GRect(0, time_start_y + hour_text_size.h + vertical_spacing, 
                              bounds.size.w, minute_text_size.h);
    graphics_draw_text(ctx, minute_text, s_time_font, minute_bounds,
                      GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
                      
    // Add AM/PM indicator when not in 24-hour mode
    if (!IS_24) {
        // Determine if it's AM or PM based on the original hour
        bool is_pm = t->tm_hour >= 12;
        char ampm_text[3];
        
        // Use strcpy instead of direct assignment
        if (is_pm) {
            strcpy(ampm_text, "PM");
        } else {
            strcpy(ampm_text, "AM");
        }
        
        // Calculate position for AM/PM text (to the right of minutes)
        GSize minute_size = graphics_text_layout_get_content_size(minute_text, s_time_font, 
                             bounds, GTextOverflowModeWordWrap, GTextAlignmentCenter);
        
        // Get size of AM/PM text for proper alignment
        GSize ampm_size = graphics_text_layout_get_content_size(ampm_text, s_ampm_font,
                           bounds, GTextOverflowModeWordWrap, GTextAlignmentLeft);
        
        int ampm_x = (bounds.size.w / 2) + (minute_size.w / 2); // Right of minutes with 5px spacing
        
        // Align bottom of AM/PM with bottom of minutes
        int ampm_y = time_start_y + hour_text_size.h + vertical_spacing + 
                    minute_text_size.h - ampm_size.h;
        
        GRect ampm_bounds = GRect(ampm_x, ampm_y, 30, 20);
        graphics_draw_text(ctx, ampm_text, s_ampm_font, ampm_bounds,
                          GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
    }
                      
    // Format and display steps at the bottom
    if (s_step_count > 0) {
        char steps_buffer[16];
        snprintf(steps_buffer, sizeof(steps_buffer), "%d", s_step_count);
        
        int steps_margin = 10;
        GRect steps_bounds = GRect(0, bounds.size.h - steps_margin - 20, 
                                bounds.size.w, 20);
        
        graphics_draw_text(ctx, steps_buffer, s_date_font, steps_bounds,
                        GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
    }
}

static void main_window_load(Window *window) {
    // Create canvas layer
    s_canvas_layer = layer_create(layer_get_bounds(window_get_root_layer(window)));
    layer_set_update_proc(s_canvas_layer, draw_watchface);
    layer_add_child(window_get_root_layer(window), s_canvas_layer);
    
    // Load fonts
    s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NDOT_FONT_51));
    s_date_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NDOT_FONT_16));
    s_ampm_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_NDOT_FONT_14));
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

static void init() {
    s_main_window = window_create();
    // Changed background color to black
    window_set_background_color(s_main_window, GColorBlack);
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload,
    });
    
    // Initialize health service
    health_service_events_subscribe(health_handler, NULL);
    
    // Get initial step count
    update_steps();
    
    window_stack_push(s_main_window, true);
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    // Deinitialize custom fonts
    fonts_unload_custom_font(s_time_font);
    fonts_unload_custom_font(s_date_font);
    fonts_unload_custom_font(s_ampm_font);

    window_destroy(s_main_window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}