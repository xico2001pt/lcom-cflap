#include "sprite.h"
#include <video_gr.h>

Sprite *create_sprite(xpm_map_t map, int16_t x, int16_t y, int xspeed, int yspeed) {
  Sprite *sp = (Sprite *) malloc(sizeof(Sprite));
  xpm_image_t img;
  if (sp == NULL)
    return NULL;
  // read the sprite pixmap
  sp->map = xpm_load(map, XPM_INDEXED, &img);
  if (sp->map == NULL) {
    free(sp);
    return NULL;
  }
  sp->width = img.width;
  sp->height = img.height;
  sp->x = x;
  sp->y = y;
  sp->xspeed = xspeed;
  sp->yspeed = yspeed;
  return sp;
}

int animate_sprite(Sprite *sp) {
  vg_clear_sprite(sp);
  sp->x += sp->xspeed;
  sp->y += sp->yspeed;
  vg_draw_sprite(sp);
  return 0;
}

int translocate_sprite(Sprite *sp, int16_t x_disp, int16_t y_disp) {
  sp->x += x_disp;
  sp->y += y_disp;
  return 0;
}

void destroy_sprite(Sprite *sp) {
  if (sp == NULL)
    return;
  if (sp->map)
    free(sp->map);
  free(sp);
  sp = NULL;
}

int vg_clear_sprite(Sprite *sprite) {
  return vg_rectangle_draw(sprite->x, sprite->y, sprite->width, sprite->height, 0);
}
