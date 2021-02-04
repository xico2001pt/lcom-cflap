#ifndef __MANAGER__
#define __MANAGER__
#include "keyboard.h"
#include "comm.h"

/** @defgroup manager manager
 * @{
 *
 * Manager
 */

#define UART_BITRATE 57600
#define UART_COM 1
#define UART_BITS_PER_CHAR 8
#define UART_STOP_BYTES 2

typedef enum {RBup, RBdown, LBup, LBdown, MOVE, KEYdown, TIMERtick, RTCupdate, UARTdata} event_type;

typedef struct {
    event_type type;
    union{
        Key key;
        struct{
            int16_t delta_x;
            int16_t delta_y;
        } mouse;
        struct{
            char *time_str;
            char *date_str;
        } rtc;
        struct{
            comm_event ev;
            comm_data data;
        } uart;
    } data;
} event_t;

/**
 * Initializes the devices
 * @return 0 on success, non-zero otherwise
 */
int init_devices();
/**
 * Terminates the devices
 * @return 0 on success, non-zero otherwise
 */
int exit_devices();

/**
 * Contains the interrupt cycle
 * @return 0 on success, non-zero otherwise
 */
int interrupt_cycle();

/**
 * Timer 0 interrupt handler
 * @return 0 on success, non-zero otherwise
 */
int timer0_int_handler();
/**
 * Keyboard interrupt handler
 * @return 0 on success, non-zero otherwise
 */
int keyboard_int_handler();
/**
 * Mouse interrupt handler
 * @return 0 on success, non-zero otherwise
 */
int mouse_int_handler();
/**
 * RTC interrupt handler
 * @return 0 on success, non-zero otherwise
 */
int rtc_int_handler();
/**
 * RTC Update interrupt handler
 * @return 0 on success, non-zero otherwise
 */
int update_int_handler();
/**
 * UART interrupt handler
 * @return 0 on success, non-zero otherwise
 */
int uart_int_handler();
/**
 * UART read handler
 * @return 0 on success, non-zero otherwise
 */
int uart_read_handler();
/**
 * RTC Alarm interrupt handler
 * @return 0 on success, non-zero otherwise
 */
int alarm_int_handler();
#endif
