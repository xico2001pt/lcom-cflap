#ifndef __UART__
#define __UART__
#include <stdint.h>

/** @defgroup uart uart
 * @{
 *
 * Uart
 */

#define COM1_IRQ 4
#define COM1_BASE_ADDR 0x3F8
#define COM2_IRQ 3
#define COM2_BASE_ADDR 0x2F8

// Register Adresses
#define RB_REG 0  /**@brief Receiver Buffer Register */
#define TH_REG 0  /**@brief Transmitter Holding Register */
#define DIV_LATCH_LSB_REG 0 /**@brief Divisor Latch LSB Register */
#define DIV_LATCH_MSB_REG 1 /**@brief Divisor Latch MSB Register*/
#define IE_REG 1 /**@brief Interrupt Enable Register */
#define II_REG 2 /**@brief Interrupt Identification Register*/
#define FC_REG 2 /**@brief FIFO Control Register*/
#define LC_REG 3 /**@brief Line Control Register*/
#define LS_REG 5 /**@brief Line Status Register*/

// LCR (Line Control Register) Control bits
#define LCR_5_BPC 0 /**@brief LCR Set 5 Bits Per Char */
#define LCR_6_BPC BIT(0) /**@brief LCR Set 6 Bits per Char*/
#define LCR_7_BPC BIT(1) /**@brief LCR Set 7 Bits per Char*/
#define LCR_8_BPC (BIT(1) | BIT(0)) /**@brief LCR Set 8 Bits per Char*/
#define LCR_1_STOP_BIT 0 /**@brief LCR Set 1 stop bit*/
#define LCR_2_STOP_BIT BIT(2) /**@brief LCR Set 2 stop bit*/
#define LCR_NO_PARITY 0 /**@brief LCR Set No Parity bit*/
#define LCR_ODD_PARITY BIT(3) /**@brief LCR Set Odd Parity bit*/
#define LCR_EVEN_PARITY (BIT(4) | BIT(3)) /**@brief LCR Set Even Parity bit*/
#define LCR_BREAK_CONTROL BIT(6) /**@brief LCR Break control*/
#define LCR_DLAB BIT(7) /**@brief LCR Divisor Latch Access*/
#define LCR_RW 0

// LSR (Line Status Register)
#define LSR_RECEIVER_DATA BIT(0)
#define LSR_OVERRUN_ERROR BIT(1)
#define LSR_PARITY_ERROR BIT(2)
#define LSR_FRAMING_ERROR BIT(3)
#define LSR_BREAK_INTERRUPT BIT(4)
#define LSR_THR_EMPTY BIT(5)
#define LSR_TRANSMITTER_EMPTY BIT(6)
#define LSR_FIFO_ERROR BIT(7)

// IER (Interrupt Enable Register)
#define IER_RECEIVED_DATA_AVAIL BIT(0)
#define IER_THR_EMPTY BIT(1)
#define IER_RECEIVER_LINE_STATUS BIT(2)

// IIR (Interrupt Identification Register)
#define IIR_NO_INT BIT(0)
//      Pending interrupts:
#define IIR_INTERRUPT_MASK (BIT(3) | BIT(2) | BIT(1))
#define IIR_RECEIVER_LINE_STATUS (BIT(2) | BIT(1))
#define IIR_RECEIVED_DATA_AVAIL BIT(2)
#define IIR_CHAR_TIMEOUT (BIT(3) | BIT(2))
#define IIR_THR_EMPTY BIT(1)
//      FIFO Stuff:
#define IIR_64BYTE_FIFO_ENABLED BIT(5)
#define IIR_FCR_SET (BIT(7) | BIT(6))

// FCR (Fifo Control Register)
#define FCR_ENABLE_BOTH_FIFO BIT(0)
#define FCR_CLEAR_RCVR_BYTES BIT(1)
#define FCR_CLEAR_XMIT_BYTES BIT(2)
#define FCR_ENABLE_64BYTE BIT(5)
#define FCR_TRIGGER_LEVEL_1 0
#define FCR_TRIGGER_LEVEL_4 BIT(6)
#define FCR_TRIGGER_LEVEL_8 BIT(7)
#define FCR_TRIGGER_LEVEL_14 (BIT(6) | BIT(7))

#define UART_DIVLATCH_CONST 115200

typedef enum {no_parity, even_parity, odd_parity} uart_parity;

/**
 * @brief Get uart Line Status
 * 
 * @param com COM line number
 * @return Line Status Register value
 */ 
uint8_t uart_get_status(uint8_t com);

/**
 * @brief Write byte into Transmitter Holding Register
 * 
 * @param com COM line number
 * @param c Byte to be written
 */
int uart_send_byte(uint8_t com, char c);

/**
 * @brief Read byte from Receiver Buffer Register
 * 
 * @param com COM line number
 * @return Receiver Buffer Register value
 */
uint8_t uart_read_byte(uint8_t com);

/**
 * @brief Read byte from Interrupt Identification Register
 * 
 * @param com COM line number
 * @return Interrupt Identification Register value
 */
uint8_t uart_get_iir(uint8_t com);

/**
 * @brief Set UART configuration
 * 
 * @param com COM line number
 * @param bitrate Bitrate value
 * @param bits_per_char Bits per char
 * @param n_stop_bits Number of stop bits
 * @param parity Type of parity bit
 * @return Return 0 upon success, non-zero otherwise
 */
int uart_set(uint8_t com, uint16_t bitrate, uint8_t bits_per_char, uint8_t n_stop_bits, uart_parity parity);

/**
 * @brief Set UART bitrate
 * 
 * @param com COM line number
 * @param bitrate Bitrate value
 * @return Return 0 upon success, non-zero otherwise
 */
int uart_set_bitrate(uint8_t com, uint16_t bitrate);

/**
 * @brief Subscribe uart interrupts
 * 
 * @param com COM line number
 * @param bit_no bit number of the device
 * @return Return 0 upon success, non-zero otherwise
 */
int uart_subscribe_int(uint8_t com, uint8_t *bit_no);

/**
 * @brief Unsubscribe uart interrupts
 * 
 * @return Return 0 upon succes, non-zero otherwise
 */
int uart_unsubscribe_int();

/**
 * @brief Read a UART register
 * 
 * @param com COM line number
 * @param reg Reg number (offset from base)
 * @return Return value read upon success, zero otherwise
 */
uint8_t uart_read_reg(uint8_t com, uint8_t reg);

/**
 * @brief Write to a UART Register
 * 
 * @param com COM line number
 * @param reg Reg number (offset from base)
 * @param byte Byte to be written
 * @return Return 0 upon success, non-zero otherwise
 */
int uart_write_reg(uint8_t com, uint8_t reg, uint8_t byte);

/**
 * @brief Get UART base register address of a given COM line
 * 
 * @param com COM line number
 * @return Return base addr upon success, 0 otherwise
 */
int uart_get_base_addr(uint8_t com);

/**
 * @brief Enable interrupts on InterruptEnableRegister
 * 
 * @param com COM line number
 * @return Return 0 upon success, non-zero otherwise
 */
int uart_enable_interrupts(uint8_t com);

/**
 * @brief Enable fifo
 * 
 * @param com COM line number
 * @param trigger_level Trigger level to be used
 * @return Return 0 upon success, non-zero otherwise
 */
int uart_enable_fifo(uint8_t com, uint8_t trigger_level);

/**
 * @brief Read (and discard) all bytes in ReceiverBufferRegister
 * 
 * @param com COM line number
 * @return Return 0 upon success, non-zero otherwise
 */
int uart_clean_rx_buffer(uint8_t com);

#endif
