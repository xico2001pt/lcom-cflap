#ifndef __QUEUE__
#define __QUEUE__
#include <stdbool.h>
#include <stdint.h>

/** @defgroup queue queue
 * @{
 *
 * Queue
 */

typedef struct{
    uint8_t *buffer;  /*< queue buffer */
    int byte_count;   /*< number of bytes in buffer */
    int buffer_size;  /*< buffer size */
} Queue;

/**
 * Creates queue with the given characteristics
 * @param buffer_size buffer size
 * @return pointer to the buffer
 */
Queue * create_queue(unsigned int buffer_size);
/**
 * Destroys the given queue, freeing the allocated memory
 * @param q queue to be destroyed
 * @return 0 on success, non-zero otherwise
 */
int delete_queue(Queue *q);
/**
 * Adds string to the given queue
 * @param q queue
 * @param str string
 * @return 0 on success, non-zero otherwise
 */
int queue_add_string(Queue *q, char str[]);
/**
 * Adds character to the given queue
 * @param q queue
 * @param c character
 * @return 0 on success, non-zero otherwise
 */
int queue_add_char(Queue *q, char c);
/**
 * Checks if the queue is full
 * @param q queue
 * @return boolean indicating if the queue is full
 */
bool queue_full(Queue *q);
/**
 * Checks if the queue is empty
 * @param q queue
 * @return boolean indicating if the queue is empty
 */
bool queue_empty(Queue *q);
/**
 * Reads a character from the queue
 * @param q queue
 * @return character read
 */
char queue_read_char(Queue *q);
/**
 * Reads a string from the queue
 * @param q queue
 * @param n_chars number of characters to be read
 * @param buffer buffer
 * @return 0 on success, non-zero otherwise
 */
int queue_read_string(Queue *q, int n_chars, char *buffer);

#endif
