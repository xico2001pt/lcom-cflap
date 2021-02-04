#include "manager.h"
#include <lcom/lcf.h>

#include "bios.h"
#include "cflap.h"
#include "i8042.h"
#include "i8254.h"
#include "keyboard.h"
#include "mouse.h"
#include "palette.h"
#include "rtc.h"
#include "timer.h"
#include "uart.h"
#include "video_gr.h"

#include "queue.h"

device_info keyboard;
device_info mouse;
device_info timer;
device_info rtc;
device_info uart;

Queue *uart_write_queue;

extern uint8_t out_byte;
extern int hook_id;
uint64_t timer_counter;
event_t events[16];
size_t n_events;

/* Coisas do mouse */
uint8_t mouse_bytes[3];
uint8_t mouse_byte_count;

/* Coisas do RTC */
char date_str[11] = "00/00/00";
char time_str[11] = "00:00:00";

static bool thr_empty = true;

int interrupt_cycle() {
  if (init_devices()) {
    printf("%s: init_devices() failed\n", __func__);
    return 1;
  }
  int r, ipc_status;
  message msg;

  bool exit = false;
  while (!exit || !queue_empty(uart_write_queue)) { /* You may want to use a different condition */
    n_events = 0;
    /* Get a request message. */
    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) { /* received notification */
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:                                 /* hardware interrupt notification */
          if (msg.m_notify.interrupts & rtc.irq_set) { /* subscribed interrupt */
            if (rtc_int_handler())
              return 1;
          }
          if (msg.m_notify.interrupts & keyboard.irq_set) { /* subscribed interrupt */
            if (keyboard_int_handler())
              return 1;
            out_byte = 0;
          }
          if (msg.m_notify.interrupts & uart.irq_set) {
            if (uart_int_handler())
              return 1;
          }
          if (msg.m_notify.interrupts & mouse.irq_set) { /* subscribed interrupt */
            if (mouse_int_handler())
              return 1;
            out_byte = 0;
          }
          else if (msg.m_notify.interrupts & timer.irq_set) {
            if (timer0_int_handler())
              return 1;
          }
          if (!exit && event_handler(events, n_events)) {
            exit = true;
          }
          while (!queue_empty(uart_write_queue) && thr_empty) {
            char c = queue_read_char(uart_write_queue);
            uart_send_byte(UART_COM, c);
            uint8_t status = uart_get_status(UART_COM);
            thr_empty = status & LSR_THR_EMPTY ? true : false;
          }
          break;
        default:
          break; /* no other notifications expected: do nothing */
      }
    }
    else { /* received a standard message, not a notification */
      /* no standard messages expected: do nothing */
    }
  }
  exit_devices();
  return 0;
}

int init_devices() {
  vg_init(0x105);
  palette_init();

  if (rtc_init()) {
    printf("%s: rtc_init failed\n", __func__);
    return 1;
  }
  rtc_set_alarm(RTC_DONT_CARE_VALUE, RTC_DONT_CARE_VALUE, 0);
  rtc = create_device(RTC_IRQ);
  if (rtc_subscribe_int(&rtc.bit_no)) {
    printf("%s: rtc_subscribe_int failed\n", __func__);
    return 1;
  }
  rtc.hook_id = hook_id;

  keyboard = create_device(KBD_IRQ);
  if (kbd_subscribe_int(&keyboard.bit_no) != OK) {
    printf("%s: kbd_subscribe_int failed\n", __func__);
    return 1;
  }
  keyboard.hook_id = hook_id;

  mouse_enable_reporting();
  mouse = create_device(MOUSE_IRQ);
  if (mouse_subscribe_int(&mouse.bit_no) != OK) {
    printf("%s: mouse_subscribe_int failed\n", __func__);
    return 1;
  }
  mouse.hook_id = hook_id;

  timer = create_device(TIMER0_IRQ);
  if (timer_subscribe_int(&timer.bit_no) != OK) {
    printf("%s: timer_subscribe_int failed\n", __func__);
    return 1;
  }
  timer.hook_id = hook_id;

  uart_write_queue = create_queue(256);
  cflap_setup_uart_queue(uart_write_queue);
  if (uart_set(UART_COM, UART_BITRATE, UART_BITS_PER_CHAR, UART_STOP_BYTES, even_parity)) {
    printf("uart_set failed\n");
    return 1;
  }
  if (uart_enable_fifo(UART_COM, FCR_TRIGGER_LEVEL_4)) {
    printf("uart_enable_fifo failed\n");
    return 1;
  }
  uart_clean_rx_buffer(UART_COM);
  uart = create_device(COM1_IRQ);
  if (uart_subscribe_int(UART_COM, &(uart.bit_no))) {
    printf("uart_subscribe_int failed\n");
    return 1;
  }
  uart.hook_id = hook_id;
  return 0;
}

int exit_devices() {
  hook_id = keyboard.hook_id;
  kbd_unsubscribe_int();
  hook_id = timer.hook_id;
  timer_unsubscribe_int();
  hook_id = rtc.hook_id;
  rtc_unsubscribe_int();
  rtc_cleanup();
  hook_id = uart.hook_id;
  uart_unsubscribe_int();
  hook_id = mouse.hook_id;
  mouse_unsubscribe_int();
  mouse_disable_reporting();
  vg_cleanup();
  return 0;
}

int timer0_int_handler() {
  event_t ev;
  ev.type = TIMERtick;
  events[n_events++] = ev;
  return 0;
}

int keyboard_int_handler() {
  static int size = 0;
  static uint8_t bytes[2];
  bytes[0] = 0;
  kbc_ih();
  bytes[size] = out_byte;

  if (out_byte == 0) {
    return 1;
  }

  if (out_byte != 0xE0) {
    Key k;
    if (!scancode_to_key(out_byte, &k)) {
      event_t ev;
      ev.type = KEYdown;
      ev.data.key = k;
      events[n_events++] = ev;
    }
    size = 0;
  }
  else
    size++;
  return 0;
}

int mouse_int_handler() {
  static bool LB = 0;
  static bool RB = 0;
  mouse_ih();
  if (out_byte & BIT(3) || mouse_byte_count != 0) {
    mouse_bytes[mouse_byte_count] = out_byte;
    mouse_byte_count++;
  }
  if (mouse_byte_count == 3) {
    mouse_byte_count = 0;
    struct packet pp = process_packets(mouse_bytes);
    event_t ev;
    ev.type = MOVE;
    if (pp.lb != LB) {
      ev.type = pp.lb ? LBdown : LBup;
      LB = pp.lb;
    }
    if (pp.rb != RB) {
      ev.type = pp.rb ? RBdown : RBup;
      RB = pp.rb;
    }

    ev.data.mouse.delta_x = pp.delta_x;
    ev.data.mouse.delta_y = pp.delta_y;
    events[n_events++] = ev;
  }
  return 0;
}

int rtc_int_handler() {
  uint8_t cause;
  rtc_read_register(RTC_REGISTER_C, &cause);

  if (cause & RTC_UF) {
    if (update_int_handler())
      return 1;
  }
  if (cause & RTC_AF) {
    if (alarm_int_handler())
      return 1;
  }

  return 0;
}

int update_int_handler() {
  if (rtc_read_date_to_string(date_str)) {
    return 1;
  }
  if (rtc_read_time_to_string(time_str)) {
    return 1;
  }
  event_t ev;
  ev.type = RTCupdate;
  ev.data.rtc.date_str = date_str;
  ev.data.rtc.time_str = time_str;
  events[n_events++] = ev;
  return 0;
}

int alarm_int_handler() {
  static bool night_mode = false;
  if (night_mode) {
    palette_set_day_mode();
  }
  else {
    palette_set_night_mode();
  }
  night_mode = !night_mode;
  return 0;
}

int uart_int_handler() {
  uint8_t iir = uart_get_iir(UART_COM);
  while (!(iir & IIR_NO_INT)) {
    switch (iir & IIR_INTERRUPT_MASK) {
      case IIR_RECEIVER_LINE_STATUS: {
        event_t ev;
        ev.type = UARTdata;
        ev.data.uart.ev = ERROR_EVENT;
        events[n_events++] = ev;
        break;
      }
      case IIR_RECEIVED_DATA_AVAIL:
      case IIR_CHAR_TIMEOUT:
        uart_read_handler();
        break;
      case IIR_THR_EMPTY: thr_empty = true; break;
      default: break;
    }
    iir = uart_get_iir(UART_COM);
  }
  return 0;
}

int uart_read_handler() {
  static int total_bytes = 2;
  static int n = 0;
  static char bytes[10];

  uint8_t status = uart_get_status(UART_COM);
  while (status & LSR_RECEIVER_DATA) {
    bytes[n++] = uart_read_byte(UART_COM);
    if (n == 2) {
      total_bytes += bytes[1] - '0';
    }
    if (n == total_bytes) {
      if (bytes[0]) {
        event_t ev;
        ev.type = UARTdata;
        ev.data.uart.ev = comm_decode_event(bytes, &(ev.data.uart.data));
        printf("got %c %c\n", bytes[0], bytes[1]);
        events[n_events++] = ev;
      }
      total_bytes = 2;
      n = 0;
    }
    status = uart_get_status(UART_COM);
  }

  return 0;
}
