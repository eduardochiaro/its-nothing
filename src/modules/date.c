#include <pebble.h>
#include <ctype.h>
#include "date.h"

void draw_date(Layer *layer, GContext *ctx, GRect location, GFont font_used) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
    
  // Get weekday and day
  char weekday_buffer[16];
  strftime(weekday_buffer, sizeof(weekday_buffer), "%a", t);
  
  // Convert weekday to uppercase
  for(int i = 0; weekday_buffer[i]; i++) {
    weekday_buffer[i] = tolower((unsigned char)weekday_buffer[i]);
  }
  
  // Create date string (weekday + day)
  char day_buffer[8];
  char date_text[24];

  snprintf(day_buffer, sizeof(day_buffer), "%d", t->tm_mday);
  snprintf(date_text, sizeof(date_text), "%s %s", weekday_buffer, day_buffer);
  
  graphics_draw_text(ctx, date_text, font_used, location,
                    GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
}