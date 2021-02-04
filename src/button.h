#ifndef __BUTTON_H__
#define __BUTTON_H__
#include <lcom/lcf.h>
#include <stdint.h>

/** @defgroup button button
 * @{
 *
 * Button
 */

#define BUTTON_WIDTH 48
#define BUTTON_BORDER_SIZE 2

typedef struct{
    int16_t x, y;             /**< horizontal and vertical position of the button */
    xpm_image_t icon_img;      /**< icon image xmp */
    uint8_t * icon_map;        /**< icon pixmap */
    uint8_t background_color;  /**< button background color */
    uint8_t hover_color;       /**< button color when the mouse is over the button */
    uint8_t selected_color;    /**< button color when the button is selected */
} Button;

/**
 * Creates a button with the given characteristics
 * @param x horizontal axis position
 * @param y vertical axis position
 * @param icon icon image xpm
 * @param background_color button background color
 * @param hover_color button color when the mouse is over the button
 * @param selected_color button color when the button is selected
 * @return pointer to the button
 */
Button * create_button(int16_t x, int16_t y, xpm_map_t icon, uint8_t background_color, uint8_t hover_color, uint8_t selected_color);

/**
 * Changes the color attributes of the button
 * @param button pointer to the button to be changed
 * @param background_color button background color
 * @param hover_color button color when the mouse is over the button
 * @param selected_color button color when the button is selected
 * @return 0 if success, non-zero otherwise
 */
int change_button_colors(Button *button, uint8_t background_color, uint8_t hover_color, uint8_t selected_color);

/**
 * Destroys the given button, freeing allocated memory
 * @param button pointer to the button to be destroyed
 */
void delete_button(Button *button);

/**
 * Draws the given button on the screen
 * @param button pointer to the button to be drawn
 * @param hover if true, draws the button with hover color
 * @param selected if true, draws the button with selected color
 * @return 0 if success, non-zero otherwise
 */
int draw_button(Button * button, bool hover, bool selected);

/**
 * Draws the given button on the screen with a specific color
 * @param button pointer to the button to be drawn
 * @param color color to be drawn
 * @return 0 if success, non-zero otherwise
 */
int draw_button_with_color(Button * button, uint8_t color);

#endif
