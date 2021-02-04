#include "input-box.h"
#include <lcom/lcf.h>

InputBox *create_input_box(size_t max_size) {
  InputBox *box = (InputBox *) malloc(sizeof(InputBox));
  char *buffer = (char *) malloc(sizeof(char) * (max_size + 1));
  box->input_str = buffer;
  box->max_input_str_len = max_size;
  box->input_str_len = 0;
  box->caps = false;
  box->lshift = false;
  box->rshift = false;
  box->changed = false;
  return box;
}

int input_box_process_key(InputBox *input, Key *k) {
  if (k->isChar) {
    if (input->input_str_len < input->max_input_str_len) {
      char c = ((k->key.character < '0' || k->key.character > '9') && (input->caps || input->lshift || input->rshift)) ? k->key.character - 32 : k->key.character;
      input->input_str[input->input_str_len] = c;
      input->input_str_len++;
      input->input_str[input->input_str_len] = 0;
      input->changed = true;
    }
  }
  else if (k->key.specialKey == BACKSPACE) {
    if (input->input_str_len > 0) {
      input->input_str[--(input->input_str_len)] = 0;
    }
    input->changed = true;
  }
  else if (k->key.specialKey == CAPSLOCK) {
    input->caps = !(input->caps);
    input->changed = true;
  }
  else if (k->key.specialKey == LSHIFTDOWN || k->key.specialKey == LSHIFTUP) {
    input->lshift = (k->key.specialKey == LSHIFTDOWN);
    input->changed = true;
  }
  else if (k->key.specialKey == RSHIFTDOWN || k->key.specialKey == RSHIFTUP) {
    input->rshift = (k->key.specialKey == RSHIFTDOWN);
    input->changed = true;
  }
  return 0;
}

void input_box_clean(InputBox *input) {
  memset(input->input_str, 0, sizeof(char) * input->max_input_str_len);
  input->input_str_len = 0;
}

int delete_input_box(InputBox *input) {
  free(input->input_str);
  free(input);
  return 0;
}
