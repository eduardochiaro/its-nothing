#include <pebble.h>

void draw_single_digit(Layer *layer, GContext *ctx, GRect location, int height, int digit, int position, GColor color, int increase_dot_size) {
  // Position if 0 align to the left, if 1 align to the right
  // 1 = dot is present, 0 = no dot
  static const uint8_t DIGIT_PATTERNS[10][7][5] = {
    // 0
    {
      {0,1,1,1,0},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {0,1,1,1,0}
    },
    // 1
    {
      {0,0,1,0,0},
      {0,1,1,0,0},
      {0,0,1,0,0},
      {0,0,1,0,0},
      {0,0,1,0,0},
      {0,0,1,0,0},
      {0,1,1,1,0}
    },
    // 2
    {
      {0,1,1,1,0},
      {1,0,0,0,1},
      {0,0,0,0,1},
      {0,0,0,1,0},
      {0,0,1,0,0},
      {0,1,0,0,0},
      {1,1,1,1,1}
    },
    // 3
    {
      {0,1,1,1,0},
      {1,0,0,0,1},
      {0,0,0,0,1},
      {0,0,1,1,0},
      {0,0,0,0,1},
      {1,0,0,0,1},
      {0,1,1,1,0}
    },
    // 4
    {
      {0,0,0,1,0},
      {0,0,1,1,0},
      {0,1,0,1,0},
      {1,0,0,1,0},
      {1,1,1,1,1},
      {0,0,0,1,0},
      {0,0,0,1,0}
    },
    // 5
    {
      {1,1,1,1,1},
      {1,0,0,0,0},
      {1,0,0,0,0},
      {1,1,1,1,0},
      {0,0,0,0,1},
      {1,0,0,0,1},
      {0,1,1,1,0}
    },
    // 6
    {
      {0,0,1,1,1},
      {0,1,0,0,0},
      {1,0,0,0,0},
      {1,1,1,1,0},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {0,1,1,1,0}
    },
    // 7
    {
      {1,1,1,1,1},
      {0,0,0,0,1},
      {0,0,0,1,0},
      {0,0,1,0,0},
      {0,1,0,0,0},
      {0,1,0,0,0},
      {0,1,0,0,0}
    },
    // 8
    {
      {0,1,1,1,0},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {0,1,1,1,0},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {0,1,1,1,0}
    },
    // 9
    {
      {0,1,1,1,0},
      {1,0,0,0,1},
      {1,0,0,0,1},
      {0,1,1,1,1},
      {0,0,0,0,1},
      {0,0,0,1,0},
      {0,1,1,0,0}
    }
  };

  if (digit < 0 || digit > 9) {
    return;
  }

  // Calculate dot size and spacing
  int total_spaces_vertical = 6; // 6 spaces between 7 rows
  int total_spaces_horizontal = 4; // 4 spaces between 5 columns
  
  // get dot size based on height available
  int dot_size = (height - total_spaces_vertical) / 7 + increase_dot_size;
  int width = (dot_size * 5) + total_spaces_horizontal;
  
  // Calculate x position based on alignment
  int x_offset;
  if (position == 0) {
    x_offset = location.origin.x + 5; //left
  } else {
    x_offset = location.origin.x + (location.size.w - width) - 5; //right
  }

  graphics_context_set_fill_color(ctx, color);

  // Draw the dots
  for (int row = 0; row < 7; row++) {
    for (int col = 0; col < 5; col++) {
      if (DIGIT_PATTERNS[digit][row][col]) {
        int x = x_offset + (col * (dot_size + 1));
        int y = location.origin.y + (row * (dot_size + 1));
        graphics_fill_circle(ctx, GPoint(x + dot_size/2, y + dot_size/2), dot_size/2);
      }
    }
  }
}