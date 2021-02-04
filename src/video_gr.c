#include "video_gr.h"
#include "bios.h"
#include "palette.h"
#include "sprite.h"
#include <lcom/lcf.h>
#include <machine/int86.h>
#include <math.h>
#include <minix/sysutil.h>

static void *video_mem; /* Process (virtual) address to which VRAM is mapped */
static void *video_buffer;
static void *font_ptr;
static unsigned h_res;           /* Horizontal resolution in pixels */
static unsigned v_res;           /* Vertical resolution in pixels */
static unsigned bits_per_pixel;  /* Number of VRAM bits per pixel */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static unsigned frame_size;
static enum xpm_image_type img_type;

void *(vg_init)(uint16_t mode) {
  vbe_mode_info_t vmi_p;
  vbe_mode_get_info(mode, &vmi_p);
  switch (mode) {
    case MODE_1024x768_INDEX: img_type = XPM_INDEXED; break;
    case MODE_640x480_DIRECT: img_type = XPM_1_5_5_5; break;
    case MODE_800x600_DIRECT: img_type = XPM_8_8_8; break;
    case MODE_1280x1024_DIRECT: img_type = XPM_5_6_5; break;
    case MODE_1152x864_DIRECT: img_type = XPM_8_8_8_8; break;
    default: img_type = INVALID_XPM;
  }
  h_res = vmi_p.XResolution;
  v_res = vmi_p.YResolution;
  bits_per_pixel = vmi_p.BitsPerPixel;
  bytes_per_pixel = bits_per_pixel != 15 ? bits_per_pixel / 8 : 2;
  unsigned phys_base_ptr = vmi_p.PhysBasePtr;
  printf("h_res: %d   v_res: %d   bits_per_pixel: %d\n", h_res, v_res, bits_per_pixel);

  frame_size = h_res * v_res * bytes_per_pixel;
  video_mem = vbe_map_vram(phys_base_ptr, 2 * frame_size);

  if (video_mem == NULL) {
    printf("Failed to map vram\n");
    return NULL;
  }

  video_buffer = (void *) ((uint8_t *) video_mem + frame_size);
  memset(video_mem, 0, 2 * frame_size);

  if (vbe_set_mode(mode)) {
    printf("vbe_set_mode failed\n");
    return NULL;
  }

  font_ptr = bios_get_font_ptr(STROKE_COLOR, TRANSPARENCY_COLOR, bytes_per_pixel);
  return video_mem;
}

void vg_cleanup() {
  vbe_set_display_start(0);
  free(font_ptr);
  vg_exit();
}

int vg_draw_horizontal_line(int16_t x, int16_t y, uint16_t len, uint32_t color) {
  uint8_t *ptr;
  ptr = video_buffer;
  ptr += (y * h_res + x) * bytes_per_pixel;

  for (int i = 0; i < len; ++i) {
    if (fit_in_screen(x + i, y))
      memcpy(ptr, &color, bytes_per_pixel);
    ptr += bytes_per_pixel;
  }
  return 0;
}

int(vg_draw_char)(char c, int16_t x, int16_t y) {
  uint8_t *ptr = font_ptr;
  ptr += c * FONT_HEIGHT * FONT_WIDTH * bytes_per_pixel;
  uint8_t *base_buffer = video_buffer;

  for (int i = 0; i < FONT_HEIGHT; ++i) {
    uint8_t *buffer = base_buffer + (y + i) * h_res + x;

    for (int j = 0; j < FONT_WIDTH; ++j) {
      uint8_t color = *ptr;
      if (color != TRANSPARENCY_COLOR && fit_in_screen(x + j, y + i)) {
        memcpy(buffer, ptr, bytes_per_pixel);
      }
      ptr += bytes_per_pixel;
      buffer += bytes_per_pixel;
    }
  }
  return 0;
}

void vg_show_buffer() {
  vbe_set_display_start(video_buffer > video_mem ? v_res : 0);
  void *tmp = video_buffer;
  video_buffer = video_mem;
  video_mem = tmp;
}

void vg_clear_buffer() {
  memset(video_buffer, 0, h_res * v_res * bytes_per_pixel);
}

int vg_rectangle_draw(int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t color) {
  for (int i = 0; i < height; ++i) {
    vg_draw_horizontal_line(x, y + i, width, color);
  }
  return 0;
}

int vg_draw_arrow(int16_t xi, int16_t yi, int16_t xf, int16_t yf, uint32_t color, int size) {
  double line_vector[2] = {xf - xi, yf - yi};
  double len = sqrt(pow(line_vector[0], 2) + pow(line_vector[1], 2));
  line_vector[0] /= len;
  line_vector[1] /= len;

  uint16_t point[2] = {(uint16_t)(xf - line_vector[0] * (STATE_RADIUS + size) + 0.5), (uint16_t)(yf - line_vector[1] * (STATE_RADIUS + size) + 0.5)};
  double normal[2] = {-line_vector[1], line_vector[0]};
  uint16_t left[2] = {(uint16_t)(point[0] + normal[0] * size + 0.5), (uint16_t)(point[1] + normal[1] * size + 0.5)};
  uint16_t right[2] = {(uint16_t)(point[0] - normal[0] * size + 0.5), (uint16_t)(point[1] - normal[1] * size + 0.5)};

  xf = (uint16_t)(xf - 34 * line_vector[0] + 0.5);
  yf = (uint16_t)(yf - 34 * line_vector[1] + 0.5);
  vg_draw_line(left[0], left[1], xf, yf, color, 4);
  vg_draw_line(right[0], right[1], xf, yf, color, 4);
  return 0;
}

int vg_draw_line(int16_t xi, int16_t yi, int16_t xf, int16_t yf, uint32_t color, uint16_t thickness) {
  int half_thickness = (int) (thickness / 2);
  if (xi == xf) {
    uint16_t y = yi < yf ? yi : yf;
    return vg_rectangle_draw(xi - half_thickness, y, thickness, abs(yf - yi), color);
  }
  if (yi == yf) {
    uint16_t x = xi < xf ? xi : xf;
    return vg_rectangle_draw(x, yi - half_thickness, abs(xf - xi), thickness, color);
  }

  double slope = (double) (yf - yi) / (xf - xi);

  for (int i = -(thickness + 1) / 2; i < half_thickness; ++i) {
    if (abs(slope) < 1) {
      double y = yi + i;
      int sign = xi < xf ? 1 : -1;
      for (uint16_t dx = 0; dx <= abs(xi - xf); ++dx) {
        uint8_t *ptr = video_buffer;
        ptr += ((int) (y + 0.5) * h_res + (xi + dx * sign)) * bytes_per_pixel;
        if (fit_in_screen(xi + dx * sign, (int) (y + 0.5)))
          memcpy(ptr, &color, bytes_per_pixel);
        y += slope * sign;
      }
    }
    else {
      double x = xi + i;
      int sign = yi < yf ? 1 : -1;
      for (uint16_t dy = 0; dy <= abs(yf - yi); ++dy) {
        uint8_t *ptr = video_buffer;
        ptr += ((yi + dy * sign) * h_res + (int) (x + 0.5)) * bytes_per_pixel;
        if (fit_in_screen((int) (x + 0.5), yi + dy * sign))
          memcpy(ptr, &color, bytes_per_pixel);
        x += 1 / (slope) *sign;
      }
    }
  }
  return 0;
}

uint8_t *vg_read_xpm(xpm_map_t xpm, xpm_image_t *img) {
  uint8_t *map;
  map = xpm_load(xpm, img_type, img);
  if (map == NULL) {
    printf("Couldn't read xpm\n");
    return NULL;
  }
  return map;
}

int vg_draw_xpm(int16_t x, int16_t y, xpm_image_t *img, uint8_t *map) {
  uint8_t *line_ptr = video_buffer;
  line_ptr += y * bytes_per_pixel * h_res;

  for (int i = 0; i < img->height; ++i) {
    uint8_t *buffer_ptr = line_ptr + x;
    for (int j = 0; j < img->width; ++j) {
      uint32_t color = 0;
      memcpy(&color, map, bytes_per_pixel);
      map += bytes_per_pixel;
      if (color != TRANSPARENCY_COLOR && fit_in_screen(x + j, y + i)) {
        memcpy(buffer_ptr, &color, bytes_per_pixel);
      }
      buffer_ptr += bytes_per_pixel;
    }
    line_ptr += bytes_per_pixel * h_res;
  }
  return 0;
}

int vg_draw_sprite(Sprite *sprite) {
  uint8_t *map = sprite->map;
  uint8_t *line_ptr = video_buffer;
  line_ptr += sprite->y * bytes_per_pixel * h_res;

  for (int i = 0; i < sprite->height; ++i) {
    uint8_t *buffer_ptr = line_ptr + (sprite->x) * bytes_per_pixel;
    for (int j = 0; j < sprite->width; ++j) {
      uint32_t color = 0;
      memcpy(&color, map, bytes_per_pixel);
      map += bytes_per_pixel;
      if (color != TRANSPARENCY_COLOR && (sprite->y + i) >= 0 && fit_in_screen(sprite->x + j, sprite->y + i)) {
        memcpy(buffer_ptr, &color, bytes_per_pixel);
      }
      buffer_ptr += bytes_per_pixel;
    }
    line_ptr += bytes_per_pixel * h_res;
  }
  return 0;
}

bool fit_in_screen(int16_t x, int16_t y) {
  return y < (int) v_res && y >= 0 && x < (int) h_res && x >= 0;
}
