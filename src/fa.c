#include "fa.h"
#include "palette.h"
#include "state-xpm.h"
#include "video_gr.h"
#include <math.h>
#include <string.h>

#define MAX_STATES 10

typedef struct {
  bool active;
  bool final;
  bool locked;
  int16_t x_pos;
  int16_t y_pos;
} state;

static int8_t initial_state = -1;
static state states[MAX_STATES];
static uint64_t nfa[MAX_STATES][MAX_STATES];
static int h_res, v_res;

static xpm_image_t state_img;
static uint8_t *state_map;
static xpm_image_t final_state_img;
static uint8_t *final_state_map;
static xpm_image_t locked_state_img;
static uint8_t *locked_state_map;
static xpm_image_t locked_final_state_img;
static uint8_t *locked_final_state_map;
static xpm_image_t self_transition_img;
static uint8_t *self_transition_map;

uint64_t fa_get_bit_from_char(char c) {
  uint64_t bit = 1;
  if (c >= 'a' && c <= 'z') {
    bit = bit << (c - 'a');
    return bit;
  }
  if (c >= 'A' && c <= 'Z') {
    bit = bit << (26 + (c - 'A'));
    return bit;
  }
  if (c >= '0' && c <= '9') {
    bit = bit << (52 + (c - '0'));
    return bit;
  }
  return 0;
}

uint64_t fa_get_state_transitions(int8_t s1, int8_t s2) {
  return nfa[s1][s2];
}

char fa_get_char_from_index(size_t index) {
  if (index >= 62) {
    return 0;
  }
  if (index >= 52) {
    return (char) ((index - 52) + '0');
  }
  if (index >= 26) {
    return (char) ((index - 26) + 'A');
  }
  return (char) (index + 'a');
}

void fa_clear() {
  memset(states, 0, sizeof(state) * MAX_STATES);
  memset(nfa, 0, sizeof(uint64_t) * MAX_STATES * MAX_STATES);
  initial_state = -1;
}

int fa_create_state(int16_t x_pos, int16_t y_pos) {
  for (int i = 0; i < MAX_STATES; ++i) {
    if (!states[i].active) {
      states[i].active = true;
      states[i].final = false;
      states[i].locked = false;
      states[i].x_pos = x_pos;
      states[i].y_pos = y_pos;
      return 0;
    }
  }
  return 1;
}

int fa_delete_state(int8_t s2) {
  if (s2 >= 0 && s2 < MAX_STATES && states[s2].active == true) {
    states[s2].active = false;
    if (initial_state == s2) {
      initial_state = -1;
    }
    for (int i = 0; i < MAX_STATES; ++i) {
      nfa[i][s2] = 0;
      nfa[s2][i] = 0;
    }
    return 0;
  }
  return 1;
}

// Create a transition going from s1 to s2 with char c
void fa_create_transition(int8_t s1, int8_t s2, char c) {
  if (s1 >= 0 && s1 < MAX_STATES && s2 >= 0 && s2 < MAX_STATES) {
    if (states[s1].active && states[s2].active) {
      uint64_t bit = fa_get_bit_from_char(c);
      if (bit) {
        nfa[s1][s2] = nfa[s1][s2] | bit;
      }
    }
  }
}

// Remove transitions going from s1 to s2
void fa_remove_transition(int8_t s1, int8_t s2, char c) {
  if (s1 >= 0 && s1 < MAX_STATES && s2 >= 0 && s2 < MAX_STATES) {
    uint64_t bit = fa_get_bit_from_char(c);
    if (bit) {
      uint64_t mask = ~bit;
      if (states[s1].active && states[s2].active) {
        nfa[s1][s2] = nfa[s1][s2] & mask;
      }
    }
  }
}

void fa_remove_all_transitions(int8_t s1, int8_t s2) {
  if (s1 >= 0 && s1 < MAX_STATES && s2 >= 0 && s2 < MAX_STATES)
    if (states[s1].active && states[s2].active)
      nfa[s1][s2] = 0;
}

int fa_set_initial_state(int8_t s1) {
  if (s1 >= 0 && s1 < MAX_STATES && states[s1].active) {
    initial_state = s1;
    return 0;
  }
  return 1;
}

int fa_toggle_final_state(int8_t s1) {
  if (s1 >= 0 && s1 < MAX_STATES && states[s1].active) {
    states[s1].final = !(states[s1].final);
    return 0;
  }
  return 1;
}

int fa_simulate(char *str) {
  if (initial_state != -1) {
    return fa_simulate_from_state(str, initial_state);
  }
  return -1;
}

// Return 0 if string passes
// Return 1 if string doesn't pass
// Return -1 if dfa isn't valid
int fa_simulate_from_state(char *str, int8_t current_state) {
  if (strlen(str) == 0) {
    return states[current_state].final ? 0 : 1;
  }
  char curr_char = str[0];
  uint64_t curr_bit = fa_get_bit_from_char(curr_char);
  str++;

  int result = 1;
  for (int i = 0; i < MAX_STATES; ++i) {
    if (nfa[current_state][i] & curr_bit) {
      int n = fa_simulate_from_state(str, i);
      result = (n == 0) ? 0 : result;
    }
  }
  return result;
}

int fa_draw() {
  if (fa_draw_transitions()) {
    return 1;
  }
  if (fa_draw_states()) {
    return 1;
  }
  return 0;
}

int fa_draw_transitions() {
  // First draw all transitions
  for (int i = 0; i < MAX_STATES; ++i) {
    if (!states[i].active)
      continue;
    for (int j = i; j < MAX_STATES; ++j) {
      if (!states[i].active || !states[j].active || (nfa[i][j] == 0 && nfa[j][i] == 0))
        continue;
      if (i == j) {
        if (fa_draw_self_transition(i)) {
          return 1;
        }
      }
      else {
        int16_t x1 = states[i].x_pos + STATE_RADIUS;
        int16_t y1 = states[i].y_pos + STATE_RADIUS;
        int16_t x2 = states[j].x_pos + STATE_RADIUS;
        int16_t y2 = states[j].y_pos + STATE_RADIUS;

        if (nfa[j][i] == 0) {
          vg_draw_line(x1, y1, x2, y2, HIGHLIGHT_COLOR, 4);
          vg_draw_arrow(x1, y1, x2, y2, HIGHLIGHT_COLOR, 10);
        }
        else if (nfa[i][j] == 0) {
          vg_draw_line(x2, y2, x1, y1, HIGHLIGHT_COLOR, 4);
          vg_draw_arrow(x2, y2, x1, y1, HIGHLIGHT_COLOR, 10);
        }
        else {
          if (fa_draw_double_transition(x1, y1, x2, y2)) {
            return 1;
          }
        }
        fa_draw_transition_chars(i, j);
      }
    }
  }
  return 0;
}

int fa_draw_self_transition(int8_t s) {
  vg_draw_xpm(states[s].x_pos + 10, states[s].y_pos - 30, &self_transition_img, self_transition_map);
  vg_draw_arrow(states[s].x_pos + 20, states[s].y_pos + 15, states[s].x_pos + 28, states[s].y_pos + 38, HIGHLIGHT_COLOR, 6);
  uint8_t n_transitions = 0;
  for (int k = 0; k < 62; ++k) {
    uint64_t bit = 1;
    bit = bit << k;
    if (nfa[s][s] & bit) {
      ++n_transitions;
      char c = fa_get_char_from_index(k);
      vg_draw_char(c, states[s].x_pos + STATE_RADIUS + 5 - FONT_WIDTH, states[s].y_pos + STATE_RADIUS - 50 - FONT_HEIGHT * (n_transitions + 1));
    }
  }
  return 0;
}

int fa_draw_double_transition(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
  const unsigned int OFFSET = 5;
  double line_vector[2] = {x2 - x1, y2 - y1};
  double len = sqrt(pow(line_vector[0], 2) + pow(line_vector[1], 2));
  line_vector[0] /= len;
  line_vector[1] /= len;
  double normal[2] = {-line_vector[1], line_vector[0]};
  double slope;
  if (line_vector[0] != 0)
    slope = line_vector[1] / line_vector[0];
  else
    slope = 9999;
  int16_t dx = OFFSET * normal[0];
  int16_t dy = OFFSET * normal[1];
  if (x2 - x1 < 0 && -CRITICAL_SLOPE <= slope && slope <= CRITICAL_SLOPE) {
    int16_t tmp = x2;
    x2 = x1;
    x1 = tmp;
    tmp = y2;
    y2 = y1;
    y1 = tmp;
  }
  vg_draw_line(x2 + dx, y2 + dy, x1 + dx, y1 + dy, HIGHLIGHT_COLOR, 4);
  vg_draw_arrow(x2 + dx, y2 + dy, x1 + dx, y1 + dy, HIGHLIGHT_COLOR, 6);
  vg_draw_line(x1 - dx, y1 - dy, x2 - dx, y2 - dy, HIGHLIGHT_COLOR, 4);
  vg_draw_arrow(x1 - dx, y1 - dy, x2 - dx, y2 - dy, HIGHLIGHT_COLOR, 6);
  return 0;
}

int fa_draw_states() {
  for (int i = 0; i < MAX_STATES; ++i) {
    if (states[i].active) {
      if (i == initial_state) {
        vg_draw_arrow(states[i].x_pos - STATE_RADIUS, states[i].y_pos + STATE_RADIUS, states[i].x_pos + STATE_RADIUS, states[i].y_pos + STATE_RADIUS, HIGHLIGHT_COLOR, 10);
        vg_draw_line(states[i].x_pos - 20, states[i].y_pos + STATE_RADIUS, states[i].x_pos + STATE_RADIUS, states[i].y_pos + STATE_RADIUS, HIGHLIGHT_COLOR, 4);
      }
      if (states[i].final)
        if (states[i].locked)
          vg_draw_xpm(states[i].x_pos, states[i].y_pos, &locked_final_state_img, locked_final_state_map);
        else
          vg_draw_xpm(states[i].x_pos, states[i].y_pos, &final_state_img, final_state_map);
      else if (states[i].locked)
        vg_draw_xpm(states[i].x_pos, states[i].y_pos, &locked_state_img, locked_state_map);
      else
        vg_draw_xpm(states[i].x_pos, states[i].y_pos, &state_img, state_map);

      if (i < 10)
        vg_draw_char('0' + (i % 10), states[i].x_pos + STATE_RADIUS - FONT_WIDTH / 2, states[i].y_pos + STATE_RADIUS - FONT_HEIGHT / 2);
    }
  }
  return 0;
}

int fa_draw_transition_chars(int8_t s1, int8_t s2) {
  /* Transitions from [s1] to [s2] */
  uint64_t transitions_from = nfa[s1][s2];
  uint8_t n_transitions_from = 0;

  /* Transitions from [s2] to [s1] */
  uint64_t transitions_to = nfa[s2][s1];
  uint8_t n_transitions_to = 0;

  int16_t middle_dist_x = (states[s2].x_pos - states[s1].x_pos) / 2;
  int16_t middle_dist_y = (states[s2].y_pos - states[s1].y_pos) / 2;
  double slope;
  if (middle_dist_x != 0)
    slope = middle_dist_y / (float) middle_dist_x;
  else
    slope = 99999;
  for (int k = 0; k < 62; ++k) {
    uint64_t bit = 1;
    bit = bit << k;

    if (transitions_from & bit) { // falta a condição da letra caber no ecra
      bool fit_in_screen = states[s1].y_pos + STATE_RADIUS + middle_dist_y - FONT_HEIGHT * (n_transitions_from + 2) > 60;
      if (fit_in_screen) {
        ++n_transitions_from;
        char c = fa_get_char_from_index(k);
        int16_t x_disp = states[s1].x_pos + STATE_RADIUS + middle_dist_x - FONT_WIDTH;
        int16_t y_disp = states[s1].y_pos + STATE_RADIUS + middle_dist_y - FONT_HEIGHT * (n_transitions_from + 1);
        if (fabs(slope) >= CRITICAL_SLOPE)
          x_disp -= CHAR_MARGIN;
        vg_draw_char(c, x_disp, y_disp);
      }
    }
    if (transitions_to & bit) {
      bool fit_in_screen = true; // Uh... just ignore this for now
      if (fit_in_screen) {
        ++n_transitions_to;
        char c = fa_get_char_from_index(k);
        int16_t x_disp = states[s1].x_pos + STATE_RADIUS + middle_dist_x - FONT_WIDTH;
        int16_t y_disp = states[s1].y_pos + STATE_RADIUS + middle_dist_y + FONT_HEIGHT * n_transitions_to;
        if (fabs(slope) >= CRITICAL_SLOPE)
          x_disp += CHAR_MARGIN + FONT_WIDTH;
        vg_draw_char(c, x_disp, y_disp);
      }
    }
  }
  return 0;
}

int fa_check_collision(int16_t x, int16_t y) {
  for (int i = MAX_STATES; i >= 0; --i) {
    if (!states[i].active)
      continue;
    if (check_circle_collision(states[i].x_pos + STATE_RADIUS, states[i].y_pos + STATE_RADIUS, STATE_RADIUS, x, y))
      return i;
  }
  return -1;
}

int fa_get_coord(int8_t s1, uint32_t *coord) {
  if (!states[s1].active)
    return 1;
  coord[0] = states[s1].x_pos;
  coord[1] = states[s1].y_pos;
  return 0;
}

int fa_translocate_state(int8_t s1, int16_t x_disp, int16_t y_disp) {
  if (!states[s1].active)
    return 1;
  states[s1].x_pos += x_disp;
  states[s1].y_pos += y_disp;

  if (states[s1].x_pos < 0) {
    states[s1].x_pos = 0;
  }
  if (states[s1].x_pos + STATE_RADIUS * 2 >= h_res) {
    states[s1].x_pos = h_res - 1 - 2 * STATE_RADIUS;
  }
  if (states[s1].y_pos < 0) {
    states[s1].y_pos = 0;
  }
  if (states[s1].y_pos + STATE_RADIUS * 2 >= v_res) {
    states[s1].y_pos = v_res - 1 - 2 * STATE_RADIUS;
  }

  return 0;
}

int fa_get_state_pos(int8_t s, int16_t *x, int16_t *y) {
  if (s >= 0 && s < MAX_STATES) {
    *x = (uint16_t) states[s].x_pos;
    *y = (uint16_t) states[s].y_pos;
    return 0;
  }
  return 1;
}

int fa_set_state_pos(int8_t s, int16_t x, int16_t y) {
  if (s >= 0 && s < MAX_STATES && states[s].active) {
    states[s].x_pos = x;
    states[s].y_pos = y;
    return 0;
  }
  return 1;
}

int fa_lock_state(int8_t s) {
  if (s >= 0 && s < MAX_STATES && states[s].active) {
    states[s].locked = true;
    return 0;
  }
  return 1;
}

int fa_unlock_state(int8_t s) {
  if (s >= 0 && s < MAX_STATES && states[s].active) {
    states[s].locked = false;
    return 0;
  }
  return 1;
}

int fa_startup(int hres, int vres) {
  fa_clear();
  h_res = hres;
  v_res = vres;
  state_map = vg_read_xpm((xpm_map_t) state_xpm, &state_img);
  locked_state_map = vg_read_xpm((xpm_map_t) locked_state_xpm, &locked_state_img);
  final_state_map = vg_read_xpm((xpm_map_t) final_state_xpm, &final_state_img);
  locked_final_state_map = vg_read_xpm((xpm_map_t) locked_final_state_xpm, &locked_final_state_img);
  self_transition_map = vg_read_xpm((xpm_map_t) self_transition_xpm, &self_transition_img);
  return 0;
}

int fa_cleanup() {
  free(state_map);
  free(final_state_map);
  free(self_transition_map);
  return 0;
}
