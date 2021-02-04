#ifndef __FA_H__
#define __FA_H__
#include <stddef.h>
#include <stdint.h>

/** @defgroup fa fa
 * @{
 *
 * Fa
 */

#define STATE_WIDTH 70
#define STATE_RADIUS 35
#define CRITICAL_SLOPE 1
#define CHAR_MARGIN 10


/**
 * Read necessary xpm and clear the fa
 * @param hres Horizontal resolution
 * @param vres Vertical resolution
 * @return Return 0 upon success, non-zero otherwise
 */
int fa_startup(int hres, int vres);

/**
 * Free all allocated memory
 * @return Return 0 upon success, non-zero otherwise
 */
int fa_cleanup();

/**
 * Gets the corresponding bit
 * @param c char to be decoded
 * @return corresponding number with active bit
 */
uint64_t fa_get_bit_from_char(char c);
/**
 * Gets the transition between two states
 * @param s1 first state
 * @param s2 second state
 * @return transitions between two states
 */
uint64_t fa_get_state_transitions(int8_t s1, int8_t s2);
/**
 * Gets the corresponding char
 * @param index bit index
 * @return corresponding char
 */
char fa_get_char_from_index(size_t index);
/**
 * Clear FA
 */
void fa_clear();
/**
 * Create state in the given position
 * @param x_pos horizontal position
 * @param y_pos vertical position
 * @return 0 if success, non-zero otherwise
 */
int fa_create_state(int16_t x_pos, int16_t y_pos);
/**
 * Delete the given state and its transitions
 * @param s2 state index
 * @return 0 if success, non-zero otherwise
 */
int fa_delete_state(int8_t s2);
/**
 * Create transition between two states
 * @param s1 state 1
 * @param s2 state 2
 * @param c symbol to be added
 */
void fa_create_transition(int8_t s1, int8_t s2, char c);
/**
 * Delete transition between two states
 * @param s1 state 1
 * @param s2 state 2
 * @param c symbol to be deleted
 */
void fa_remove_transition(int8_t s1, int8_t s2, char c);
/**
 * Delete all transitions between two states
 * @param s1 state 1
 * @param s2 state 2
 */
void fa_remove_all_transitions(int8_t s1, int8_t s2);
/**
 * Set the given state as initial
 * @param s1 state to be set
 * @return 0 if success, non-zero otherwise
 */
int fa_set_initial_state(int8_t s1);
/**
 * Set the given state as final
 * @param s1 state to be set
 * @return 0 if success, non-zero otherwise
 */
int fa_toggle_final_state(int8_t s1);
/**
 * Simulates the given string form the given state
 * @param str string to be processed
 * @param current_state state where the simulation will begin from
 * @return 0 if success, non-zero otherwise
 */
int fa_simulate_from_state(char *str, int8_t current_state);
/**
 * Simulates the given string form the initial state
 * @param str string to be processed
 * @return 0 if success, non-zero otherwise
 */
int fa_simulate(char *str);

/**
 * Draws the FA on the screen
 * @return 0 if success, non-zero otherwise
 */
int fa_draw();
/**
 * Draws all FA states on the screen
 * @return 0 if success, non-zero otherwise
 */
int fa_draw_states();
/**
 * Draws all FA transitions on the screen
 * @return 0 if success, non-zero otherwise
 */
int fa_draw_transitions();
/**
 * Draws self transitions from the given state
 * @param s state
 * @return 0 if success, non-zero otherwise
 */
int fa_draw_self_transition(int8_t s);
/**
 * Draws double directions transitions
 * @param x1 horizontal position form the state 1
 * @param y1 vertical position form the state 1
 * @param x2 horizontal position form the state 2
 * @param y2 vertical position form the state 2
 * @return 0 if success, non-zero otherwise
 */
int fa_draw_double_transition(int16_t x1, int16_t y1, int16_t x2, int16_t y2);
/**
 * Draws the transition characters from the given states
 * @param s1 state 1
 * @param s2 state 2
 * @return 0 if success, non-zero otherwise
 */
int fa_draw_transition_chars(int8_t s1, int8_t s2);

/**
 * Checks if the given position collides with any state
 * @param x horizontal position
 * @param y vertical position
 * @return state index, -1 if none
 */
int fa_check_collision(int16_t x, int16_t y);
/**
 * Gets coordinates of the given state
 * @param s1 state
 * @param coord coordinated returned as parameter
 * @return 0 if success, non-zero otherwise
 */
int fa_get_coord(int8_t s1, uint32_t *coord);
/**
 * Translocates the given state
 * @param s1 state
 * @param x_disp horizontal displacement
 * @param y_disp vertical displacement
 * @return 0 if success, non-zero otherwise
 */
int fa_translocate_state(int8_t s1, int16_t x_disp, int16_t y_disp);
/**
 * Gets the position of the given state
 * @param s state
 * @param x horizontal position
 * @param y vertical position
 * @return 0 if success, non-zero otherwise
 */
int fa_get_state_pos(int8_t s, int16_t *x, int16_t *y);
/**
 * Sets the position of the given state
 * @param s state
 * @param x horizontal position
 * @param y vertical position
 * @return 0 if success, non-zero otherwise
 */
int fa_set_state_pos(int8_t s, int16_t x, int16_t y);

/**
 * Lock a given state
 * @param s state
 * @return 0 if success, non-zero otherwise
 */
int fa_lock_state(int8_t s);

/**
 * Unlock a given state
 * @param s state
 * @return 0 if success, non-zero otherwise
 */
int fa_unlock_state(int8_t s);

#endif
