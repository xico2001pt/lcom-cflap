#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include <stdbool.h>
#include <stdint.h>

/** @defgroup keyboard keyboard
 * @{
 *
 * Keyboard
 */

typedef enum {ESC, BACKSPACE, ENTER, LSHIFTUP, LSHIFTDOWN, RSHIFTUP, RSHIFTDOWN, CAPSLOCK} SpecialKey;

typedef union {
  char character;         /**< alfa-numerical character */
  SpecialKey specialKey;  /**< non-alfa-numerical key */
} KeyInfo;

typedef struct {
  bool isChar;  /**< indicates if the key is an alfa-numerical character */
  KeyInfo key;  /**< key info */
} Key;

/**
 * Maps the scancode to the corresponding key
 * @param scancode Scancode to be mapped
 * @param key Struct where the key will be returned
 * @return 0 on success, non-zero otherwise
 */
int scancode_to_key(uint8_t scancode, Key *key);

/**
 * Subscribe to keyboard interrupts
 * @param bit_no bit number of the device
 * @return 0 on success, non-zero otherwise
 */
int kbd_subscribe_int(uint8_t *bit_no);

/**
 * Unsubscribe keyboard interrupts
 * @return 0 on success, non-zero otherwise
 */
int kbd_unsubscribe_int();


#endif /* _KEYBOARD_H_ */
