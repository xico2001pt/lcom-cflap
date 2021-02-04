#include "uart.h"
#include "utils.h"
#include <lcom/lcf.h>

int uart_set(uint8_t com, uint16_t bitrate, uint8_t bits_per_char, uint8_t n_stop_bits, uart_parity parity) {
  if (uart_set_bitrate(com, bitrate)) {
    return 1;
  }
  uint8_t lcr_byte = 0;
  switch (bits_per_char) {
    case 5: lcr_byte |= LCR_5_BPC; break;
    case 6: lcr_byte |= LCR_6_BPC; break;
    case 7: lcr_byte |= LCR_7_BPC; break;
    case 8: lcr_byte |= LCR_8_BPC; break;
    default: return 1;
  }
  switch (n_stop_bits) {
    case 1: lcr_byte |= LCR_1_STOP_BIT; break;
    case 2: lcr_byte |= LCR_2_STOP_BIT; break;
    default: return 1;
  }
  switch (parity) {
    case no_parity: lcr_byte |= LCR_NO_PARITY; break;
    case even_parity: lcr_byte |= LCR_EVEN_PARITY; break;
    case odd_parity: lcr_byte |= LCR_ODD_PARITY; break;
    default: return 1;
  }
  uart_write_reg(com, LC_REG, lcr_byte);
  return 0;
}

int uart_set_bitrate(uint8_t com, uint16_t bitrate) {
  uint8_t lcr_byte = uart_read_reg(com, LC_REG);
  uint16_t div_latch = UART_DIVLATCH_CONST / bitrate;
  uart_write_reg(com, LC_REG, lcr_byte | LCR_DLAB);
  uart_write_reg(com, DIV_LATCH_MSB_REG, (div_latch >> 8) & 0xFF);
  uart_write_reg(com, DIV_LATCH_LSB_REG, div_latch & 0xFF);
  uart_write_reg(com, LC_REG, lcr_byte);
  return 0;
}

uint8_t uart_read_reg(uint8_t com, uint8_t reg) {
  if (reg > 7) {
    return 0;
  }
  int base = uart_get_base_addr(com);
  if (!base) {
    return 0;
  }
  uint8_t byte;
  if (util_sys_inb(base + reg, &byte)) {
    return 1;
  }
  return byte;
}

int uart_write_reg(uint8_t com, uint8_t reg, uint8_t byte) {
  if (reg > 7) {
    return 1;
  }
  int base = uart_get_base_addr(com);
  if (!base) {
    return 1;
  }
  if (sys_outb(base + reg, byte)) {
    return 1;
  }
  return 0;
}

int uart_get_base_addr(uint8_t com) {
  switch (com) {
    case 1: return COM1_BASE_ADDR;
    case 2: return COM2_BASE_ADDR;
    default: return 0; // return NULL
  }
  return 0;
}

int hook_id;
int uart_subscribe_int(uint8_t com, uint8_t *bit_no) {
  if (uart_enable_interrupts(com)) {
    return 1;
  }
  switch (com) {
    case 1: hook_id = COM1_IRQ; break;
    case 2: hook_id = COM2_IRQ; break;
    default: return 1;
  }
  *bit_no = (uint8_t) hook_id;
  if (sys_irqsetpolicy(hook_id, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id) != OK) {
    printf("%s: sys_irqsetpolicy() failed\n", __func__);
    return 1;
  }
  return 0;
}

int uart_unsubscribe_int() {
  if (sys_irqrmpolicy(&hook_id) != OK) {
    printf("%s: sys_irqrmpolicy() failed\n", __func__);
    return 1;
  }
  return 0;
}

int uart_enable_interrupts(uint8_t com) {
  if (uart_write_reg(com, IE_REG, IER_RECEIVED_DATA_AVAIL | IER_THR_EMPTY | IER_RECEIVER_LINE_STATUS)) {
    return 1;
  }
  return 0;
}

int uart_enable_fifo(uint8_t com, uint8_t trigger_level) {
  uint8_t byte = 0 | FCR_ENABLE_BOTH_FIFO | FCR_ENABLE_64BYTE | FCR_CLEAR_RCVR_BYTES | FCR_CLEAR_XMIT_BYTES | trigger_level;
  if (uart_write_reg(com, FC_REG, byte)) {
    return 1;
  }
  return 0;
}

uint8_t uart_get_status(uint8_t com) {
  return uart_read_reg(com, LS_REG);
}

int uart_send_byte(uint8_t com, char c) {
  return uart_write_reg(com, TH_REG, c);
}

uint8_t uart_read_byte(uint8_t com) {
  return uart_read_reg(com, RB_REG);
}

uint8_t uart_get_iir(uint8_t com) {
  return uart_read_reg(com, II_REG);
}

int uart_clean_rx_buffer(uint8_t com) {
  uint8_t status = uart_get_status(com);
  while (status & LSR_RECEIVER_DATA) {
    if (uart_read_byte(com)) {
      return 1;
    }
    status = uart_get_status(com);
  }
  return 0;
}
