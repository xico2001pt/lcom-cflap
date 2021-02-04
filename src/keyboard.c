#include <i8042.h>
#include <kbc.h>
#include <keyboard.h>
#include <lcom/lcf.h>
#include <minix/sysutil.h>

int scancode_to_key(uint8_t scancode, Key *key) {
  if (!key)
    return 1;
  if (scancode == 0x00)
    return 2;

  bool found = true;
  // Search for special keys
  key->isChar = false;
  if (scancode == ESC_MAKECODE)
    key->key.specialKey = ESC;
  else if (scancode == BACKSPACE_MAKECODE)
    key->key.specialKey = BACKSPACE;
  else if (scancode == ENTER_MAKECODE)
    key->key.specialKey = ENTER;
  else if (scancode == LSHIFT_MAKECODE)
    key->key.specialKey = LSHIFTDOWN;
  else if (scancode == LSHIFT_BREAKCODE)
    key->key.specialKey = LSHIFTUP;
  else if (scancode == RSHIFT_MAKECODE)
    key->key.specialKey = RSHIFTDOWN;
  else if (scancode == RSHIFT_BREAKCODE)
    key->key.specialKey = RSHIFTUP;
  else if (scancode == CAPSLOCK_MAKECODE)
    key->key.specialKey = CAPSLOCK;
  else
    found = false;

  if (found)
    return 0;

  // Search for characters
  char seq1[10] = {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'};
  char seq2[9] = {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l'};
  char seq3[7] = {'z', 'x', 'c', 'v', 'b', 'n', 'm'};

  key->isChar = true;
  if (scancode >= N1_MAKECODE && scancode <= N9_MAKECODE) // Number between 1 and 9
    key->key.character = '1' + (scancode - N1_MAKECODE);
  else if (scancode == N0_MAKECODE)
    key->key.character = '0';
  else if (scancode >= Q_MAKECODE && scancode <= P_MAKECODE) // Chars of first sequence
    key->key.character = seq1[scancode - Q_MAKECODE];
  else if (scancode >= A_MAKECODE && scancode <= L_MAKECODE) // Chars of second sequence
    key->key.character = seq2[scancode - A_MAKECODE];
  else if (scancode >= Z_MAKECODE && scancode <= M_MAKECODE) // Chars of third sequence
    key->key.character = seq3[scancode - Z_MAKECODE];
  else // Non acceptable key
    return 3;
  return 0;
}

void(kbc_ih)() {
  while (1) {
    if (kbc_read_outb() == 0) {
      break;
    }
    tickdelay(micros_to_ticks(DELAY_US));
  }
}

int hook_id;
int kbd_subscribe_int(uint8_t *bit_no) {
  hook_id = KBD_IRQ;
  *bit_no = (uint8_t) hook_id;
  if (sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
    printf("%s: sys_irqsetpolicy() failed\n", __func__);
    return 1;
  }
  return 0;
}

int kbd_unsubscribe_int() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("%s: sys_irqrmpolicy() failed\n", __func__);
    return 1;
  }
  return 0;
}
