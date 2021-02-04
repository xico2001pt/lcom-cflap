#include "palette.h"
#include "bios.h"
#include <lcom/lcf.h>

int palette_init() {
  if (palette_set_bpp(8)) {
    return 1;
  }
  if (palette_load_default()) {
    return 1;
  }
  return 0;
}

int palette_set_day_mode() {
  palette_set_color(BACKGROUND_COLOR, 0xF8F1F1);
  return 0;
}

int palette_set_night_mode() {
  palette_set_color(BACKGROUND_COLOR, 0xBDBBB6);
  return 0;
}

int palette_load_default() {
  if (palette_set_color(TRANSPARENCY_COLOR, CHROMA_KEY) ||
      palette_set_color(BACKGROUND_COLOR, 0xF8F1F1) || // Background
      palette_set_color(HIGHLIGHT_COLOR, 0x16697A) ||  // Header Color
      palette_set_color(STROKE_COLOR, 0x4B4B4B) ||     // Border color
      palette_set_color(EXIT_COLOR, 0xFFB49F) ||
      palette_set_color(HOVER_COLOR, 0xD8D1D1) ||
      palette_set_color(EXIT_HOVER_COLOR, 0xDB635A) ||
      palette_set_color(SELECTED_COLOR, 0xA8A1A1) ||
      palette_set_color(CONNECTION_PENDING_COLOR, 0xEADE92) ||
      palette_set_color(CONNECTION_ACCEPTED_COLOR, 0xD1F187) ||
      palette_set_color(STATE_COLOR, 0xFFA528) ||
      palette_set_color(LOCKED_STATE_COLOR, 0xCC8420)) {
    return 1;
  }
  return 0;
}

int palette_set_bpp(uint8_t bits_per_pixel) {
  if (bits_per_pixel > 8 || bits_per_pixel < 1) {
    return 1;
  }
  struct reg86 r;
  memset(&r, 0, sizeof(r));
  r.intno = 0x10;
  r.ax = 0x4F08;
  r.bl = 0x00;
  r.bh = 8;
  if (bios_call(&r)) {
    printf("bios call failed\n");
    return 1;
  }
  return 0;
}

int palette_set_color(uint8_t index, uint32_t color) {
  struct reg86 r;
  memset(&r, 0, sizeof(r));
  r.intno = 0x10;
  r.ax = 0x1010;
  r.bx = index;
  r.dh = (color >> 16) & 0xFF; // Red
  r.ch = (color >> 8) & 0xFF;  // Green
  r.cl = color & 0xFF;         // Blue

  if (bios_call(&r)) {
    printf("BIOS call failed\n");
    return 1;
  }
  return 0;
}

uint32_t palette_read_color(uint8_t index) {
  struct reg86 r;
  memset(&r, 0, sizeof(r));
  r.intno = 0x10;
  r.ax = 0x1015;
  r.bx = index;
  if (bios_call(&r)) {
    printf("BIOS call failed\n");
    return 1;
  }
  printf("R:0x%x G:0x%x B:0x%x\n", r.dh, r.ch, r.cl);
  return 0;
}
