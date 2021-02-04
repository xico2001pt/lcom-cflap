#include "cflap.h"
#include "bios.h"
#include "button-xpm.h"
#include "button.h"
#include "comm.h"
#include "fa.h"
#include "input-box.h"
#include "keyboard.h"
#include "manager.h"
#include "mouse-xpm.h"
#include "palette.h"
#include "title_screen.h"
#include "utils.h"
#include "video_gr.h"
#include <lcom/lcf.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static Sprite *mouse;
static InputBox *input_box;
static Button *buttons[BUTTONS_NUMBER];
static Queue *uart_write_queue;
static xpm_image_t title_screen_img;
static uint8_t *title_screen_map;

static int exit_cflap = 0;

static char date_str[] = "00/00/00";
static char time_str[] = "00:00:00";
static char last_result[20] = "";
static cflap_state current_state = TITLE_SCREEN;
static cflap_tool current_tool = SELECT_TOOL;
static bool create_transition = true;
static int state1, state2;

/* Uart stuff */
static bool uart_request_sent = false, uart_request_received = false;
static bool uart_connected = false;
static bool state_moved = false;
static int user2_locked_state1 = -1, user2_locked_state2 = -1;

int event_handler(event_t events[], size_t n_events) {
  for (size_t i = 0; i < n_events; ++i) {
    event_t *event = &events[i];

    switch (event->type) {
      case KEYdown:
        kbd_event_handler(event);
        break;
      case TIMERtick:
        timer_event_handler(event);
        break;
      case RTCupdate:
        rtc_event_handler(event);
        break;
      case UARTdata:
        uart_event_handler(event);
        break;
      case RBup:
      case RBdown:
      case LBup:
      case LBdown:
      case MOVE:
        mouse_event_handler(event);
        break;
      default:
        break;
    }
  }
  if (exit_cflap) {
    uart_send_ev(END_COMM);
  }
  return exit_cflap;
}

int mouse_event_handler(event_t *event) {
  if (current_state != RUN && current_state != AWAIT_TRANSITION_KEY) {
    mouse_update_position(event);
    if(mouse_check_button_clicks(event)){
      return 0;
    }
  }
  if (current_tool == SELECT_TOOL) {
    if (current_state == DRAW && event->type == LBdown) {
      state1 = fa_check_collision(mouse->x, mouse->y);
      if (state1 != -1 && not_locked(state1)) {
        lock_state(state1);
        current_state = MOVEMENT;
      }
      else{
        state1 = -1;
      }
    }
    else if (current_state == MOVEMENT) {
      mouse_move_state(event);
    }
  }
  if (current_tool == STATE_TOOL) {
    mouse_process_state_tool(event);
  }
  if (current_tool == TRANSITION_TOOL) {
    mouse_process_transition_tool(event);
  }
  return 0;
}

int mouse_update_position(event_t *event) {
  translocate_sprite(mouse, event->data.mouse.delta_x, -(event->data.mouse.delta_y));
  mouse->x = mouse->x > SCREEN_WIDTH - 1 ? SCREEN_WIDTH - 1 : (mouse->x < 0 ? 0 : mouse->x);
  mouse->y = mouse->y > SCREEN_HEIGHT - 1 ? SCREEN_HEIGHT - 1 : (mouse->y < 0 ? 0 : mouse->y);
  return 0;
}

int mouse_check_button_clicks(event_t *event) {
  if (event->type == LBdown) {
    for (int i = 0; i < BUTTONS_NUMBER; ++i) {
      if (check_rectangle_collision(mouse->x, mouse->y, 1, 1,
                                    buttons[i]->x, buttons[i]->y, BUTTON_WIDTH, BUTTON_WIDTH)) {
        unlock_state(state1);
        unlock_state(state2);
	if (i == SHARE_TOOL) {
          share_button_click();
          return 1;
        }
        current_tool = i;
        switch (current_tool) {
          case RUN_TOOL: current_state = RUN; break;
          case EXIT: exit_cflap = 1; break;
          default: break;
        }
        return 1;
      }
    }
  }
  return 0;
}

int mouse_move_state(event_t *event) {
  state_moved = true;
  if (event->type == LBup) {
    unlock_state(state1);
    state1 = -1;
    current_state = DRAW;
  }
  fa_translocate_state(state1, event->data.mouse.delta_x, -(event->data.mouse.delta_y));
  return 0;
}

int mouse_process_state_tool(event_t *event) {
  if (event->type == LBdown) {
    fa_create_state(mouse->x - STATE_RADIUS, mouse->y - STATE_RADIUS);
    uart_send_ev_pos(CREATE_STATE, mouse->x - STATE_RADIUS, mouse->y - STATE_RADIUS);
  }
  if (event->type == RBdown) {
    int state_num = fa_check_collision(mouse->x, mouse->y);
    if (state_num != -1 && not_locked(state_num)){
      fa_delete_state(state_num);
      uart_send_ev_state(DELETE_STATE, state_num);
    }
  }
  return 0;
}

int mouse_process_transition_tool(event_t *event) {
  if (current_state == DRAW && (event->type == LBdown || event->type == RBdown)) {
    state1 = fa_check_collision(mouse->x, mouse->y);
    if (state1 != -1 && not_locked(state1)) {
      lock_state(state1);
      current_state = TRANSITION;
      create_transition = event->type == LBdown; // True if create, false if delete
    }
    else{
      state1 = -1;
    }
  }
  else if (current_state == TRANSITION && event->type == (create_transition ? LBup : RBup)) {
    state2 = fa_check_collision(mouse->x, mouse->y);
    current_state = AWAIT_TRANSITION_KEY;
    if (state2 == -1 || !not_locked(state2)) {
      current_state = DRAW;
      unlock_state(state1);
      unlock_state(state2);
      state1 = -1;
      state2 = -1;
    }
    else if(not_locked(state2)){
      lock_state(state2);
    }
  }
  return 0;
}

int share_button_click() {
  if (uart_request_received) {
    uart_request_received = false;
    uart_connected = true;
    change_button_colors(buttons[SHARE_TOOL], CONNECTION_ACCEPTED_COLOR, HOVER_COLOR, SELECTED_COLOR);
    fa_clear();
    uart_send_privileged_ev(INIT_COMM_ACCEPT);
  }
  else if (uart_connected) {
    uart_send_privileged_ev(END_COMM);
    uart_connected = false;
    uart_request_received = false;
    uart_request_sent = false;
    user2_locked_state1 = -1;
    user2_locked_state2 = -1;
    change_button_colors(buttons[SHARE_TOOL], BACKGROUND_COLOR, HOVER_COLOR, SELECTED_COLOR);
  }
  else if (!uart_request_sent) {
    uart_send_privileged_ev(INIT_COMM);
  }
  return 0;
}

int kbd_event_handler(event_t *event) {
  if (current_state == TITLE_SCREEN) {
    current_state = DRAW;
    return 0;
  }
  if (current_state == DRAW || current_state == TRANSITION) {
    kbd_process_shortcut(event);
    return 0;
  }
  else if (current_state == RUN) {
    kbd_write_to_input_box(event);
    if (!event->data.key.isChar) {
      if (event->data.key.key.specialKey == ENTER) {
        int res = fa_simulate(input_box->input_str);
        switch (res) {
          case 0: strcpy(last_result, "Passed"); break;
          case 1: strcpy(last_result, "Failed"); break;
          case -1: strcpy(last_result, "Invalid FA"); break;
        }
      }
    }
  }
  else if (current_state == AWAIT_TRANSITION_KEY) {
    kbd_write_to_input_box(event);
    if (!event->data.key.isChar) {
      if (event->data.key.key.specialKey == ENTER) {
        for (size_t i = 0; i < input_box->input_str_len; ++i) {
          char letter = input_box->input_str[i];
          if (create_transition)
            fa_create_transition(state1, state2, letter);
          else
            fa_remove_transition(state1, state2, letter);
          uart_send_ev_symbol(create_transition ? CREATE_TRANSITION : DELETE_TRANSITION, letter);
        }
        current_state = DRAW;
        unlock_state(state1);
        unlock_state(state2);
        input_box_clean(input_box);
      }
      else if (event->data.key.key.specialKey == ESC) {
        current_tool = TRANSITION_TOOL;
        unlock_state(state1);
        unlock_state(state2);
      }
    }
  }
  return 0;
}

int kbd_write_to_input_box(event_t *event) {
  if (event->data.key.isChar) {
    input_box_process_key(input_box, &(event->data.key));
    if (input_box->changed) {
      strcpy(last_result, "");
      input_box->changed = false;
    }
  }
  else {
    if (event->data.key.key.specialKey == ESC) {
      current_state = DRAW;
      current_tool = SELECT_TOOL;
      input_box_clean(input_box);
      strcpy(last_result, "");
    }
    else {
      input_box_process_key(input_box, &(event->data.key));
      if (input_box->changed) {
        strcpy(last_result, "");
        input_box->changed = false;
      }
    }
  }
  return 0;
}

int kbd_process_shortcut(event_t *event) {
  if (event->data.key.isChar) {
    current_state = DRAW;
    char c = event->data.key.key.character;
    switch (c) {
      case 'i': {
        int s = fa_check_collision(mouse->x, mouse->y);
        if (s != -1) {
          fa_set_initial_state(s);
          uart_send_ev_state(SET_INITIAL_STATE, s);
        }
        break;
      }
      case 'f': {
        int s = fa_check_collision(mouse->x, mouse->y);
        if (s != -1) {
          fa_toggle_final_state(s);
          uart_send_ev_state(SET_FINAL_STATE, s);
        }
        break;
      }
      case '1': current_tool = SELECT_TOOL; break;
      case '2': current_tool = STATE_TOOL; break;
      case '3': current_tool = TRANSITION_TOOL; break;
      case '4':
        current_tool = RUN_TOOL;
        current_state = RUN;
        break;
      case '5': {
        share_button_click();
      }
    }
    if (c >= '1' && c <= '5') {
      unlock_state(state1);
      unlock_state(state2);
    }
  }
  return 0;
}

int timer_event_handler(event_t *event) {
  draw_canvas();
  switch (current_state) {
    case TITLE_SCREEN: {
      vg_draw_xpm(0, 0, &title_screen_img, title_screen_map);
      break;
    }
    case MOVEMENT:
      if (state_moved) {
        state_moved = false;
        int16_t x, y;
        fa_get_state_pos(state1, &x, &y);
        uart_send_ev_pos(MOVE_STATE, x, y);
      }
    case DRAW:
      draw_header();
      vg_draw_sprite(mouse);
      break;
    case AWAIT_TRANSITION_KEY:
      draw_header();
      draw_inputbox();
      break;
    case TRANSITION: {
      uint32_t coord[2];
      fa_get_coord(state1, coord);
      vg_draw_line(coord[0] + STATE_RADIUS, coord[1] + STATE_RADIUS, mouse->x, mouse->y, create_transition ? HIGHLIGHT_COLOR : EXIT_HOVER_COLOR, 4);
      draw_header();
      vg_draw_sprite(mouse);
      break;
    }
    case RUN:
      draw_header();
      draw_inputbox();
      break;
  }
  vg_show_buffer();
  return 0;
}

int rtc_event_handler(event_t *event) {
  strcpy(time_str, event->data.rtc.time_str);
  strcpy(date_str, event->data.rtc.date_str);
  return 0;
}

int uart_event_handler(event_t *event) {
  switch (event->data.uart.ev) {
    case INIT_COMM:
      change_button_colors(buttons[SHARE_TOOL], CONNECTION_PENDING_COLOR, HOVER_COLOR, SELECTED_COLOR);
      uart_request_received = true;
      uart_send_privileged_ev(INIT_COMM_AKH);
      break;
    case INIT_COMM_AKH:
      uart_request_sent = true;
      change_button_colors(buttons[SHARE_TOOL], CONNECTION_PENDING_COLOR, HOVER_COLOR, SELECTED_COLOR);
      break;
    case INIT_COMM_ACCEPT:
      fa_clear();
      uart_connected = true;
      change_button_colors(buttons[SHARE_TOOL], CONNECTION_ACCEPTED_COLOR, HOVER_COLOR, SELECTED_COLOR);
      break;
    case END_COMM:
      uart_connected = false;
      uart_request_received = false;
      uart_request_sent = false;
      fa_unlock_state(user2_locked_state1);
      fa_unlock_state(user2_locked_state2);
      user2_locked_state1 = -1;
      user2_locked_state2 = -1;
      change_button_colors(buttons[SHARE_TOOL], BACKGROUND_COLOR, HOVER_COLOR, SELECTED_COLOR);
      break;
    case ERROR_EVENT:
      fa_clear();
      break;
    case CREATE_STATE:
      fa_create_state(event->data.uart.data.data.pos.x, event->data.uart.data.data.pos.y);
      break;
    case DELETE_STATE:
      fa_delete_state(event->data.uart.data.data.state_num);
      break;
    case MOVE_STATE:
      printf("s:%d (%d,%d)\n", user2_locked_state1, event->data.uart.data.data.pos.x, event->data.uart.data.data.pos.y);
      if (user2_locked_state1 != -1) {
        fa_set_state_pos(user2_locked_state1, event->data.uart.data.data.pos.x, event->data.uart.data.data.pos.y);
      }
      break;
    case LOCK_STATE:
      printf("LOCK %d\n", event->data.uart.data.data.state_num);
      if (user2_locked_state1 != -1) {
        user2_locked_state2 = event->data.uart.data.data.state_num;
        fa_lock_state(user2_locked_state2);
      }
      else {
        user2_locked_state1 = event->data.uart.data.data.state_num;
        fa_lock_state(user2_locked_state1);
      }
      break;
    case UNLOCK_STATE:
      fa_unlock_state(event->data.uart.data.data.state_num);
      user2_locked_state1 = (user2_locked_state1 == event->data.uart.data.data.state_num) ? -1 : user2_locked_state1;
      user2_locked_state2 = (user2_locked_state2 == event->data.uart.data.data.state_num) ? -1 : user2_locked_state2;
      break;
    case CREATE_TRANSITION:
      if (user2_locked_state1 != -1 && user2_locked_state2 != -1) {
        fa_create_transition(user2_locked_state1, user2_locked_state2, event->data.uart.data.data.state_num);
      }
      break;
    case DELETE_TRANSITION:
      if (user2_locked_state1 != -1 && user2_locked_state2 != -1) {
        fa_remove_transition(user2_locked_state1, user2_locked_state2, event->data.uart.data.data.state_num);
      }
      break;
    case SET_FINAL_STATE:
      fa_toggle_final_state(event->data.uart.data.data.state_num);
      break;
    case SET_INITIAL_STATE:
      fa_set_initial_state(event->data.uart.data.data.state_num);
      break;
    default:
      return 1;
  }
  return 0;
}

int draw_header() {
  vg_rectangle_draw(0, 0, SCREEN_WIDTH, HEADER_HEIGHT, HIGHLIGHT_COLOR);
  vg_rectangle_draw(RTC_X_POS, RTC_Y_POS, RTC_WIDTH, RTC_HEIGHT, BACKGROUND_COLOR);
  draw_datetime();

  for (int i = 0; i < 6; ++i) {
    bool hover = check_rectangle_collision(mouse->x, mouse->y, 1, 1,
                                           buttons[i]->x, buttons[i]->y, BUTTON_WIDTH, BUTTON_WIDTH);
    bool selected = i == current_tool;
    draw_button(buttons[i], hover, selected);
  }
  return 0;
}

int draw_inputbox() {
  uint16_t box_x = 320, box_y = 6;
  vg_rectangle_draw(box_x, box_y, 500, 48, STROKE_COLOR);
  vg_rectangle_draw(box_x + 2, box_y + 2, 496, 44, BACKGROUND_COLOR);
  uint16_t x = box_x + 10;
  uint16_t y = box_y + 10;
  for (size_t i = 0; i < input_box->input_str_len; ++i) {
    vg_draw_char(input_box->input_str[i], x, y);
    x += FONT_WIDTH;
  }
  x = box_x + 10 + FONT_WIDTH * (MAX_INPUT_STR_SIZE / 2 - strlen(last_result));
  y += FONT_HEIGHT;
  for (size_t i = 0; i < strlen(last_result); ++i) {
    vg_draw_char(last_result[i], x, y);
    x += FONT_WIDTH;
  }
  return 0;
}

int draw_datetime() {
  uint16_t x = RTC_X_POS + 10;
  for (size_t i = 0; i < strlen(time_str); ++i) {
    vg_draw_char(date_str[i], x, 14);
    vg_draw_char(time_str[i], x, 30);
    x += FONT_WIDTH;
  }
  return 0;
}

int draw_canvas() {
  vg_rectangle_draw(0, HEADER_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - HEADER_HEIGHT, BACKGROUND_COLOR);
  fa_draw();
  return 0;
}

int lock_state(int s) {
  if (s != -1) {
    fa_lock_state(s);
    uart_send_ev_state(LOCK_STATE, s);
  }
  return 0;
}

int unlock_state(int s) {
  if (s != -1) {
    fa_unlock_state(s);
    uart_send_ev_state(UNLOCK_STATE, s);
  }
  return 0;
}

bool not_locked(uint8_t s) {
  return (s != user2_locked_state1) && (s != user2_locked_state2);
}

int uart_send_ev(comm_event ev) {
  if (uart_connected) {
    comm_add_event_to_queue(uart_write_queue, ev);
  }
  return 0;
}
int uart_send_privileged_ev(comm_event ev) {
  comm_add_event_to_queue(uart_write_queue, ev);
  return 0;
}
int uart_send_ev_state(comm_event ev, uint8_t state) {
  if (uart_connected) {
    comm_data d;
    d.type = STATE_NUM;
    d.data.state_num = state;
    comm_add_event_with_data_to_queue(uart_write_queue, ev, &d);
  }
  return 0;
}
int uart_send_ev_symbol(comm_event ev, char symb) {
  if (uart_connected) {
    comm_data d;
    d.type = SYMBOL;
    d.data.symbol = symb;
    comm_add_event_with_data_to_queue(uart_write_queue, ev, &d);
  }
  return 0;
}
int uart_send_ev_pos(comm_event ev, uint16_t x, uint16_t y) {
  if (uart_connected) {
    comm_data d;
    d.type = XY_COORD;
    d.data.pos.x = x;
    d.data.pos.y = y;
    comm_add_event_with_data_to_queue(uart_write_queue, ev, &d);
  }
  return 0;
}

int cflap_startup() {
  fa_startup(SCREEN_WIDTH, SCREEN_HEIGHT);
  mouse = create_sprite((xpm_map_t) mouse_xpm, 500, 500, 0, 0);
  input_box = create_input_box(MAX_INPUT_STR_SIZE + 1);
  title_screen_map = vg_read_xpm((xpm_map_t) title_screen_xpm, &title_screen_img);

  buttons[SELECT_TOOL] = create_button(10, 6, (xpm_map_t) mouse_icon_xpm, BACKGROUND_COLOR, HOVER_COLOR, SELECTED_COLOR);
  buttons[STATE_TOOL] = create_button(10 + 57 * 1, 6, (xpm_map_t) create_state_xpm, BACKGROUND_COLOR, HOVER_COLOR, SELECTED_COLOR);
  buttons[TRANSITION_TOOL] = create_button(10 + 57 * 2, 6, (xpm_map_t) transition_xpm, BACKGROUND_COLOR, HOVER_COLOR, SELECTED_COLOR);
  buttons[RUN_TOOL] = create_button(10 + 57 * 3, 6, (xpm_map_t) run_xpm, BACKGROUND_COLOR, HOVER_COLOR, SELECTED_COLOR);
  buttons[SHARE_TOOL] = create_button(10 + 57 * 4, 6, (xpm_map_t) share_xpm, BACKGROUND_COLOR, HOVER_COLOR, SELECTED_COLOR);
  buttons[5] = create_button(966, 6, (xpm_map_t) exit_xpm, EXIT_COLOR, EXIT_HOVER_COLOR, HOVER_COLOR);
  return 0;
}
int cflap_setup_uart_queue(Queue *q) {
  uart_write_queue = q;
  return 0;
}

int cflap_cleanup() {
  fa_cleanup();
  delete_input_box(input_box);
  for (int i = 0; i < BUTTONS_NUMBER; ++i) {
    delete_button(buttons[i]);
  }
  destroy_sprite(mouse);
  free(title_screen_map);
  return 0;
}
