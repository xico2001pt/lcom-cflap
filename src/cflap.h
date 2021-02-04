#ifndef __CFLAP__
#define __CFLAP__
#include <stddef.h>
#include "manager.h"

/** @defgroup cflap cflap
 * @{
 *
 * CFLAP
 */

//////////////////////////////////////////
//      Dimensions                      //
//////////////////////////////////////////
#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define HEADER_HEIGHT 60
#define RTC_X_POS 860
#define RTC_Y_POS 6
#define RTC_WIDTH 90
#define RTC_HEIGHT 48

#define MAX_INPUT_STR_SIZE 50
#define BUTTONS_NUMBER 6

typedef enum {TITLE_SCREEN, DRAW, RUN, TRANSITION, MOVEMENT, AWAIT_TRANSITION_KEY} cflap_state;
typedef enum {SELECT_TOOL, STATE_TOOL, TRANSITION_TOOL, RUN_TOOL, SHARE_TOOL, EXIT, NO_TOOL} cflap_tool;

/**
 * Handles the events given as parameter
 * @param events events that will be handled
 * @param n_events number of events
 * @return 0 if success, non-zero otherwise
 */
int event_handler(event_t events[], size_t n_events);

/* Device specific event_handlers*/
/**
 * Mouse event handler
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int mouse_event_handler(event_t *event);
/**
 * Keyboard event handler
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int kbd_event_handler(event_t *event);
/**
 * Timer event handler
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int timer_event_handler(event_t *event);
/**
 * RTC event handler
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int rtc_event_handler(event_t *event);
/**
 * UART event handler
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int uart_event_handler(event_t *event);

/* Mouse */
/**
 * Updates the mouse position
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int mouse_update_position(event_t *event);
/**
 * Checks if any button is clicked
 * @param event event to be handled
 * @return 1 if a button was pressed, 0 otherwise
 */
int mouse_check_button_clicks(event_t *event);
/**
 * Updates a state position
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int mouse_move_state(event_t *event);
/**
 * Processes an event while state tool is selected
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int mouse_process_state_tool(event_t *event);
/**
 * Processes an event while transition tool is selected
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int mouse_process_transition_tool(event_t *event);

/* Keyboard */
/**
 * Processes a shortcut key
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int kbd_process_shortcut(event_t *event);
/**
 * Processes a key while input box is active
 * @param event event to be handled
 * @return 0 if success, non-zero otherwise
 */
int kbd_write_to_input_box(event_t *event);

/* Uart */
/**
 * Send an event through UART
 * @param ev event to be handled
 * @return 0 if success, non-zero otherwise
 */
int uart_send_ev(comm_event ev);
/**
 * Send a privileged event through UART
 * @param ev event to be handled
 * @return 0 if success, non-zero otherwise
 */
int uart_send_privileged_ev(comm_event ev);
/**
 * Send a state event through UART
 * @param ev ev event to be handled
 * @param state state to be processed
 * @return 0 if success, non-zero otherwise
 */
int uart_send_ev_state(comm_event ev, uint8_t state);
/**
 * Send a symbol event through UART
 * @param ev ev event to be handled
 * @param symb symbol to be processed
 * @return 0 if success, non-zero otherwise
 */
int uart_send_ev_symbol(comm_event ev, char symb);
/**
 * Send position event through UART
 * @param ev ev event to be handled
 * @param x horizontal position
 * @param y vertical position
 * @return 0 if success, non-zero otherwise
 */
int uart_send_ev_pos(comm_event ev, uint16_t x, uint16_t y);

/**
 * Processes the share button click
 * @return 0 if success, non-zero otherwise
 */
int share_button_click();
/**
 * Lock the given states, so the other user can't move it
 * @param s state
 * @return 0 if success, non-zero otherwise
 */
int lock_state(int s);
/**
 * Unlock the given states, so the other user can't move it
 * @param s state
 * @return 0 if success, non-zero otherwise
 */
int unlock_state(int s);
/**
 * Checks if the given state is locked
 * @param s state
 * @return 0 if success, non-zero otherwise
 */
bool not_locked(uint8_t s);

/**
 * Draws the header on the screen
 * @return 0 if success, non-zero otherwise
 */
int draw_header();
/**
 * Draws the canvas on the screen
 * @return 0 if success, non-zero otherwise
 */
int draw_canvas();
/**
 * Draws the input box on the screen
 * @return 0 if success, non-zero otherwise
 */
int draw_inputbox();
/**
 * Draws the datetime on the screen
 * @return 0 if success, non-zero otherwise
 */
int draw_datetime();

/**
 * Initializes the CFLAP program
 * @return 0 if success, non-zero otherwise
 */
int cflap_startup();
/**
 * Initializes the given UART queue
 * @param q queue to be initialized
 * @return 0 if success, non-zero otherwise
 */
int cflap_setup_uart_queue(Queue *q);
/**
 * Terminates the CFLAP program
 * @return 0 if success, non-zero otherwise
 */
int cflap_cleanup();

#endif
