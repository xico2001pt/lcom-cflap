#include "queue.h"
#include "string.h"
#include <lcom/lcf.h>

Queue *create_queue(unsigned int buffer_size) {
  Queue *q = (Queue *) malloc(sizeof(Queue));
  q->buffer = (uint8_t *) malloc(sizeof(char) * buffer_size);
  memset(q->buffer, 0, sizeof(char) * buffer_size);
  q->buffer_size = buffer_size;
  q->byte_count = 0;
  return q;
}

int delete_queue(Queue *q) {
  free(q->buffer);
  free(q);
  return 0;
}

int queue_add_string(Queue *q, char str[]) {
  while ((*str) && (q->byte_count < q->buffer_size - 1)) {
    q->buffer[q->byte_count] = (*str);
    ++(q->byte_count);
    q->buffer[q->byte_count] = 0;
    str++;
  }
  return 0;
}

int queue_add_char(Queue *q, char c) {
  if (q->byte_count < q->buffer_size - 1) {
    q->buffer[q->byte_count] = c;
    ++(q->byte_count);
    q->buffer[q->byte_count] = 0;
  }
  return 0;
}

char queue_read_char(Queue *q) {
  if (q->byte_count == 0) {
    return 0;
  }
  char c = q->buffer[0];
  for (int i = 0; i < q->buffer_size - 1; ++i) {
    q->buffer[i] = q->buffer[i + 1];
  }
  (q->byte_count)--;
  return c;
}

int queue_read_string(Queue *q, int n_chars, char *buffer) {
  if (q->byte_count == 0 || q->byte_count < n_chars) {
    return 1;
  }
  memcpy(buffer, q->buffer, n_chars * sizeof(char));
  for (int i = 0; i < q->buffer_size - 1; ++i) {
    if (i + n_chars < q->buffer_size) {
      q->buffer[i] = q->buffer[i + n_chars];
    }
  }
  (q->byte_count) -= n_chars;
  return 0;
}

bool queue_full(Queue *q) {
  return q->byte_count == q->buffer_size;
}
bool queue_empty(Queue *q) {
  return q->byte_count == 0;
}
