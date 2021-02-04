#ifndef __UTILS_H__
#define __UTILS_H__

/** @defgroup utils utils
 * @{
 *
 * LCOM's utility functions
 */

#include <stdint.h>
#include <stdbool.h> 

#ifndef BIT
#  define BIT(n) (1 << (n))
#endif

/**
 * @brief Returns the LSB of a 2 byte integer
 *
 * @param val input 2 byte integer
 * @param lsb address of memory location to be updated with val's LSB
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_get_LSB)(uint16_t val, uint8_t *lsb);

/**
 * @brief Returns the MSB of a 2 byte integer
 *
 * @param val input 2 byte integer
 * @param msb address of memory location to be updated with val's LSB
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_get_MSB)(uint16_t val, uint8_t *msb);

/**
 * @brief Invokes sys_inb() system call but reads the value into a uint8_t variable.
 *
 * @param port the input port that is to be read
 * @param value address of 8-bit variable to be update with the value read
 * @return Return 0 upon success and non-zero otherwise
 */
int (util_sys_inb)(int port, uint8_t *value);

/**
 * Returns whether or not a given scandcode if make or break
 * @param scancode 
 * @return true if make, false if break
 */
bool scancode_is_make(uint8_t scancode);

typedef struct{
  uint8_t bit_no;    /**< bit number of the device */
  uint32_t irq_set;  /**< IRQ set */
  int hook_id;       /**< hook id */
} device_info;

/**
 * Creates the device with the given characteristics
 * @param bit_no bit number of the device
 * @return device struct
 */
device_info create_device(uint8_t bit_no);

/**
 * Checks if two rectangles collide
 * @param x1 rectangle 1 top left corner horizontal position
 * @param y1 rectangle 1 top left corner vertical position
 * @param w1 rectangle 1 width
 * @param h1 rectangle 1 height
 * @param x2 rectangle 2 top left corner horizontal position
 * @param y2 rectangle 2 top left corner vertical position
 * @param w2 rectangle 2 width
 * @param h2 rectangle 2 height
 * @return boolean indicating if the rectangles collide
 */
bool check_rectangle_collision(int16_t x1, int16_t y1, uint16_t w1, uint16_t h1, int16_t x2, int16_t y2, uint16_t w2, uint16_t h2);

/**
 * Checks if the given point is inside the circle
 * @param circle_x circle center horizontal position
 * @param circle_y circle center vertical position
 * @param radius circle radius
 * @param x point horizontal position
 * @param y point vertical position
 * @return boolean indicating the given point is inside the circle
 */
bool check_circle_collision(int16_t circle_x, int16_t circle_y, uint16_t radius, int16_t x, int16_t y);

/**
 * Counts the number of bits set to 1
 * @param n number
 * @return number of bits set to 1
 */
unsigned int count_set_bits(uint64_t n);
#endif
