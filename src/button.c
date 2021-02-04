#include "button.h"
#include "video_gr.h"

static uint8_t border_color = 3;

Button *create_button(int16_t x, int16_t y, xpm_map_t icon, uint8_t background_color, uint8_t hover_color, uint8_t selected_color) {
  Button *b = (Button *) malloc(sizeof(Button));
  xpm_image_t img;
  uint8_t *map = vg_read_xpm(icon, &img);
  if (map == NULL) {
    return NULL;
  }
  b->x = x;
  b->y = y;
  b->icon_img = img;
  b->icon_map = map;
  b->background_color = background_color;
  b->hover_color = hover_color;
  b->selected_color = selected_color;
  return b;
}

void delete_button(Button *button) {
  free(button);
}

int change_button_colors(Button *button, uint8_t background_color, uint8_t hover_color, uint8_t selected_color) {
  button->background_color = background_color;
  button->hover_color = hover_color;
  button->selected_color = selected_color;
  return 0;
}

int draw_button(Button *button, bool hover, bool selected) {
  uint8_t color = selected ? button->selected_color : (hover ? button->hover_color : button->background_color);
  return draw_button_with_color(button, color);
}

int draw_button_with_color(Button *button, uint8_t color) {
  if (vg_rectangle_draw(button->x, button->y, BUTTON_WIDTH, BUTTON_WIDTH, border_color)) {
    return 1;
  }
  if (vg_rectangle_draw(button->x + BUTTON_BORDER_SIZE, button->y + BUTTON_BORDER_SIZE,
                        BUTTON_WIDTH - 2 * BUTTON_BORDER_SIZE, BUTTON_WIDTH - 2 * BUTTON_BORDER_SIZE, color)) {
    return 1;
  }
  if (vg_draw_xpm(button->x, button->y, &(button->icon_img), button->icon_map)) {
    return 1;
  }
  return 0;
}
