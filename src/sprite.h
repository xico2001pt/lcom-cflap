#ifndef _SPRITE_H_
#define _SPRITE_H_
#include <lcom/lcf.h>

/** @defgroup sprite sprite
 * @{
 *
 * Sprite
 */

typedef struct {
    int16_t x, y;            /**< current position */
    uint16_t width, height;   /**< dimensions */
    int32_t xspeed, yspeed;  /**< current speed */
    uint8_t *map;        /**< pixmap */
} Sprite;

/**
 * Creates a sprite with the given characteristics
 * @param map image xpm
 * @param x horizontal axis position
 * @param y vertical axis position
 * @param xspeed horizontal speed
 * @param yspeed vertical speed
 * @return pointer to the sprite
 */
Sprite * create_sprite(xpm_map_t map, int16_t x, int16_t y, int32_t xspeed, int32_t yspeed);

/**
 * Animates the given sprite, possibly changing its position
 * @param sp sprite to be updated
 * @return 0 if success, non-zero otherwise
 */
int animate_sprite(Sprite *sp);

/**
 * Translocates the given sprite
 * @param sp sprite
 * @param x_disp horizontal displacement
 * @param y_disp vertical displacement
 * @return 0 if success, non-zero otherwise
 */
int translocate_sprite(Sprite *sp, int16_t x_disp, int16_t y_disp);

/**
 * Destroys the given sprite, freeing allocated memory
 * @param sp sprite to be destroyed
 */
void destroy_sprite(Sprite *sp);

/**
 * Clear the given sprite on screen
 * @param sprite sprite
 * @return 0 if success, non-zero otherwise
 */
int vg_clear_sprite(Sprite * sprite);

#endif //_SPRITE_H_
