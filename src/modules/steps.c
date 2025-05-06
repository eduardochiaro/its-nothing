#include <pebble.h>

void draw_steps(Layer *layer, GContext *ctx, GRect location, GFont font_used, int step_count) {
  if (step_count > 0) {
    char steps_buffer[16];
    snprintf(steps_buffer, sizeof(steps_buffer), "%d", step_count);
    
    graphics_draw_text(ctx, steps_buffer, font_used, location, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
}