#include "comm.h"
#include "string.h"
#include <lcom/lcf.h>

int comm_add_event_to_queue(Queue *q, comm_event ev) {
  char c = 0;
  switch (ev) {
    case INIT_COMM: c = 'I'; break;
    case INIT_COMM_AKH: c = 'K'; break;
    case INIT_COMM_ACCEPT: c = 'A'; break;
    case END_COMM: c = 'E'; break;
    default: return 1;
  }
  queue_add_char(q, c);
  queue_add_char(q, '0');
  return 0;
}

int comm_add_event_with_data_to_queue(Queue *q, comm_event ev, comm_data *data) {
  char c = 0;
  char n = 0;
  switch (ev) {
    case MOVE_STATE:
      c = 'M';
      n = '4';
      break;
    case CREATE_STATE:
      c = 'C';
      n = '4';
      break;
    case DELETE_STATE:
      c = 'D';
      n = '1';
      break;
    case LOCK_STATE:
      c = 'L';
      n = '1';
      break;
    case UNLOCK_STATE:
      c = 'U';
      n = '1';
      break;
    case CREATE_TRANSITION:
      c = 'T';
      n = '1';
      break;
    case DELETE_TRANSITION:
      c = 't';
      n = '1';
      break;
    case SET_FINAL_STATE:
      c = 'F';
      n = '1';
      break;
    case SET_INITIAL_STATE:
      c = 'i';
      n = '1';
      break;
    default: return 1;
  }
  queue_add_char(q, c);
  queue_add_char(q, n);
  char str[6];
  switch (data->type) {
    case STATE_NUM:
    case SYMBOL:
      str[0] = data->data.state_num;
      break;
    case XY_COORD:
      memcpy(str, &(data->data.pos.x), 2);
      memcpy(str + 2, &(data->data.pos.y), 2);
      break;
    default:
      return 1;
  }
  for (int i = 0; i < (n - '0'); ++i) {
    queue_add_char(q, str[i]);
  }
  return 0;
}

comm_event comm_decode_event(char *bytes, comm_data *data) {
  char c = bytes[0];
  comm_event ev;
  switch (c) {
    case 'I': ev = INIT_COMM; break;
    case 'K': ev = INIT_COMM_AKH; break;
    case 'A': ev = INIT_COMM_ACCEPT; break;
    case 'E': ev = END_COMM; break;
    case 'M':
      ev = MOVE_STATE;
      data->type = XY_COORD;
      memcpy(&(data->data.pos.x), bytes + 2, 2);
      memcpy(&(data->data.pos.y), bytes + 4, 2);
      break;
    case 'C':
      ev = CREATE_STATE;
      data->type = XY_COORD;
      memcpy(&(data->data.pos.x), bytes + 2, 2);
      memcpy(&(data->data.pos.y), bytes + 4, 2);
      break;
    case 'D':
      ev = DELETE_STATE;
      data->type = STATE_NUM;
      data->data.state_num = bytes[2];
      break;
    case 'L':
      ev = LOCK_STATE;
      data->type = STATE_NUM;
      data->data.state_num = bytes[2];
      break;
    case 'U':
      ev = UNLOCK_STATE;
      data->type = STATE_NUM;
      data->data.state_num = bytes[2];
      break;
    case 'T':
      ev = CREATE_TRANSITION;
      data->type = SYMBOL;
      data->data.symbol = bytes[2];
      break;
    case 't':
      ev = DELETE_TRANSITION;
      data->type = SYMBOL;
      data->data.symbol = bytes[2];
      break;
    case 'F':
      ev = SET_FINAL_STATE;
      data->type = STATE_NUM;
      data->data.state_num = bytes[2];
      break;
    case 'i':
      ev = SET_INITIAL_STATE;
      data->type = STATE_NUM;
      data->data.state_num = bytes[2];
      break;
    default: ev = ERROR_EVENT;
  }
  return ev;
}
