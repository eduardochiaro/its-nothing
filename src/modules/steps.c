#include <pebble.h>

void draw_steps(Layer *layer, GContext *ctx, GRect location, GFont font_used, int step_count) {
  HealthMetric metric = HealthMetricStepCount;
  time_t start = time_start_of_today();
  time_t end = time(NULL);
  
  // Check if health data is available
  HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
  if (step_count > 0 && mask && HealthServiceAccessibilityMaskAvailable) {
    char steps_buffer[16];
    snprintf(steps_buffer, sizeof(steps_buffer), "%d", step_count);
    
    graphics_draw_text(ctx, steps_buffer, font_used, location, GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
  }
}