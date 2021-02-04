#ifndef __COMM__
#define __COMM__
#include <stdint.h>
#include "queue.h"

/** @defgroup comm comm
 * @{
 *
 * Comm
 */

typedef enum {INIT_COMM, INIT_COMM_AKH, INIT_COMM_ACCEPT, END_COMM, MOVE_STATE,
     CREATE_STATE, DELETE_STATE, LOCK_STATE, UNLOCK_STATE, CREATE_TRANSITION,
     DELETE_TRANSITION, SET_FINAL_STATE, SET_INITIAL_STATE, ERROR_EVENT} comm_event;
typedef enum {NO_DATA, XY_COORD, STATE_NUM, SYMBOL} comm_data_type;

typedef struct{
    comm_data_type type;
    union
    {
        struct
        {
            int16_t x;
            int16_t y;
        } pos;
        uint8_t state_num;    
        uint8_t symbol;    
    } data;
} comm_data;

/**
 * Add event to the given queue
 * @param q queue to be changed
 * @param ev event to be added
 * @return 0 if success, non-zero otherwise
 */
int comm_add_event_to_queue(Queue *q, comm_event ev);
/**
 * Add event with data to the given queue
 * @param q queue to be changed
 * @param ev event to be added
 * @param data data to be added
 * @return 0 if success, non-zero otherwise
 */
int comm_add_event_with_data_to_queue(Queue *q, comm_event ev, comm_data *data);
/**
 * Decodes the given data
 * @param bytes bytes to be decoded
 * @param data data to be filled
 * @return communication event
 */
comm_event comm_decode_event(char *bytes, comm_data *data);

#endif
