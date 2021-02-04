#ifndef __INPUT_BOX__
#define __INPUT_BOX__
#include "keyboard.h"
#include <string.h>

/** @defgroup input-box input-box
 * @{
 *
 * Input-box
 */

typedef struct{
    char *input_str;           /**< string written in the box */
    size_t input_str_len;      /**< string length */
    size_t max_input_str_len;  /**< maximum string length allowed */
    bool caps;                 /**< indicates if caps is active */
    bool lshift;               /**< indicates if lshift is active */
    bool rshift;               /**< indicates if rshift is active */
    bool changed;              /**< indicates if the input box changed */
} InputBox;

/**
 * Creates an input box with the given characteristics
 * @param max_size maximum string length allowed
 * @return pointer to the input box
 */
InputBox * create_input_box(size_t max_size);
/**
 * Processes the given key
 * @param input input box
 * @param k key
 * @return 0 if success, non-zero otherwise
 */
int input_box_process_key(InputBox *input, Key *k);
/**
 * Cleans the given input box
 * @param input input box
 */
void input_box_clean(InputBox *input);
/**
 * Destroys the given input box, freeing allocated memory
 * @param input input box
 * @return 0 if success, non-zero otherwise
 */
int delete_input_box(InputBox *input);

#endif
