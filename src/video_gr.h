#ifndef __VIDEO_GR__
#define __VIDEO_GR__
#include "utils.h"
#include "sprite.h"
#include "fa.h"

/** @defgroup video_gr video_gr
 * @{
 *
 * Video Graphics
 */

#define FONT_WIDTH 8
#define FONT_HEIGHT 16

/**
 * @brief Draw a XPM image
 * 
 * @param x x coordinate of top left corner of the image
 * @param y y coordinate of top left corner of the image
 * @param img Pointer to xpm_image_t struct
 * @param map Pointer to image pixmap
 * @return Return 0 upon success, non-zero otherwise
 */
int vg_draw_xpm(int16_t x, int16_t y, xpm_image_t *img, uint8_t *map);

/**
 * @brief Draw line from (xi,yi) to (xf,yf)
 * 
 * @param xi Initial x 
 * @param yi Initial y
 * @param xf Final x
 * @param yf Final y
 * @param color Color
 * @param thickness Line Thickness
 * @return Return 0 upon success, non-zero otherwise
 */
int vg_draw_line(int16_t xi, int16_t yi, int16_t xf, int16_t yf, uint32_t color, uint16_t thickness);

/**
 * @brief Draw an arrow at the end of a line from (xi,yi) to (xf,yf)
 * 
 * @param xi - Initial x
 * @param yi - Initial y
 * @param xf Final x
 * @param yf Final y
 * @param color Color
 * @param size Arrow size
 * @return Return 0 upon success, non-zero otherwise
 */
int vg_draw_arrow(int16_t xi, int16_t yi, int16_t xf, int16_t yf, uint32_t color, int size);

/**
 * @brief Read an xpm_map_t
 * 
 * @param xpm Pointer to xpm map
 * @param img Pointer to xpm_image_t to be filled
 * @return Return xpm pixmap pointer upon success, NULL otherwise
 */
uint8_t * vg_read_xpm(xpm_map_t xpm, xpm_image_t *img);

/**
 * @brief Draw an sprite
 * 
 * @param sprite Pointer to sprite
 * @return Return 0 upon success, non-zero otherwise
 */
int vg_draw_sprite(Sprite * sprite);

/**
 * @brief Do a page flip, showing buffer content
 */
void vg_show_buffer();

/**
 * @brief Draw a char
 * 
 * @param x x coordinate of the top left corner of the char
 * @param y y coordinate of the top left corner of the char
 * @return Return 0 upon success, non-zero otherwise
 */
int vg_draw_char(char c, int16_t x, int16_t y);

/**
 * @brief Reset video graphics to it's initial state and free all the memory used
 */
void vg_cleanup();

/*
 * @brief Completly clear the buffer (setting every value to 0)
 */
void vg_clear_buffer();

/**
 * Draw rectangle with the given characteristics
 * @param x horizontal position
 * @param y vertical position
 * @param width width
 * @param height height
 * @param color color
 * @return Return 0 upon success, non-zero otherwise
 */
int vg_rectangle_draw(int16_t x, int16_t y, uint16_t width, uint16_t height, uint32_t color);

/*
 * @brief Check if coordinates (x,y) are inside the screen boundries
 * @param x x coordinate
 * @param y y coordinate
 * @return True if (x,y) are inside the screen and false otherwise
 */
bool fit_in_screen(int16_t x, int16_t y);

#endif //__VBE__
