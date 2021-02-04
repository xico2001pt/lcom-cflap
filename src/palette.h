#ifndef __PALETTE_H__
#define __PALETTE_H__
#include <stdint.h>

/** @defgroup palette palette
 * @{
 *
 * Palette
 */

#define CHROMA_KEY 0x00B140
#define TRANSPARENCY_COLOR 0
#define BACKGROUND_COLOR 1
#define HIGHLIGHT_COLOR 2
#define STROKE_COLOR 3
#define EXIT_COLOR 4
#define HOVER_COLOR 5
#define EXIT_HOVER_COLOR 6
#define SELECTED_COLOR 7
#define CONNECTION_PENDING_COLOR 8
#define CONNECTION_ACCEPTED_COLOR 9
#define STATE_COLOR 10
#define LOCKED_STATE_COLOR 11

/**
 * Initializes palette
 * @return 0 on success, non-zero otherwise
 */
int palette_init();

/**
 * Loads palette with default colors
 * @return 0 on success, non-zero otherwise
 */
int palette_load_default();

/**
 * Sets palette bits per pixel
 * @param bits_per_pixel bits per pixel
 * @return 0 on success, non-zero otherwise
 */
int palette_set_bpp(uint8_t bits_per_pixel);

/**
 * Sets the given color in the given index
 * @param index index
 * @param color color to be set
 * @return 0 on success, non-zero otherwise
 */
int palette_set_color(uint8_t index, uint32_t color);

/**
 * Gets the color in the given index of the palette
 * @param index index of the color
 * @return 0 on success, non-zero otherwise
 */
uint32_t palette_read_color(uint8_t index);

/**
 * Sets palette night mode
 * @return 0 on success, non-zero otherwise
 */
int palette_set_night_mode();
/**
 * Sets palette day mode
 * @return 0 on success, non-zero otherwise
 */
int palette_set_day_mode();

#endif
