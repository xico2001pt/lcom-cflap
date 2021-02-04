#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

/** @defgroup i8042 i8042
 * @{
 * 
 * Constansts for programming the i8042 KBC
 */ 


#define KBD_IRQ 1 /**< Keyboard IRQ line */
#define MOUSE_IRQ 12 /**< Mouse IRQ line */
#define DELAY_US 20000 /**< Delay between outb reads */

#define KBC_OUT_BUF 0x60 /**< KBC output buffer port */
#define ESC_BREAKCODE 0x81 /**< Breakcode of ESC key */

#define KBC_RD_CMD 0x20 /**< KBC read command*/
#define KBC_WR_CMD 0x60 /**< KBC write command*/
#define KBC_ST_REG 0x64 /**< KBC status register */
#define KBC_CMD_REG 0x64 /**< KBC command register */

/** KBC status commands bits */
#define KBC_PAR_ERR BIT(7)
#define KBC_TO_ERR BIT(6)
#define KBC_AUX BIT(5)
#define KBC_ST_IBF BIT(1)
#define KBC_ST_OBF BIT(0)

/** Keyboard special keys scancodes */
#define ESC_BREAKCODE 0x81 /**< Breakcode of ESC key */
#define LSHIFT_BREAKCODE 0xaa /**< Breakcode of LSHIFT key */
#define RSHIFT_BREAKCODE 0xb6 /**< Breakcode of RSHIFT key */
#define ESC_MAKECODE 0x01 /**< Makecode of ESC key */
#define BACKSPACE_MAKECODE 0x0e /**< Makecode of BACKSPACE key */
#define ENTER_MAKECODE 0x1c /**< Makecode of ENTER key */
#define LSHIFT_MAKECODE 0x2a /**< Makecode of LSHIFT key */
#define RSHIFT_MAKECODE 0x36 /**< Makecode of RSHIFT key */
#define CAPSLOCK_MAKECODE 0x3a /**< Makecode of CAPSLOCK key */

/** Keyboard character keys scancodes */
#define N1_MAKECODE 0x02 /**< Makecode of number 1 key */
#define N9_MAKECODE 0x0a /**< Makecode of number 9 key */
#define N0_MAKECODE 0x0b /**< Makecode of number 0 key */
#define Q_MAKECODE 0x10 /**< Makecode of letter Q key */
#define P_MAKECODE 0x19 /**< Makecode of letter P key */
#define A_MAKECODE 0x1e /**< Makecode of letter A key */
#define L_MAKECODE 0x26 /**< Makecode of letter L key */
#define Z_MAKECODE 0x2c /**< Makecode of letter Z key */
#define M_MAKECODE 0x32 /**< Makecode of letter M key */




#define DATA_PACKET_BYTES 3

#define DISABLE_MOUSE 0xA7
#define ENABLE_MOUSE 0xA8
#define WRITE_BYTE_TO_MOUSE 0xD4
#define DISABLE_DATA_REPORTING 0xF5
#define ENABLE_DATA_REPORTING 0xF4
#define READ_DATA 0xEB
#define SET_STREAM_MODE 0xEA
#define SET_DEFAULT 0xF6

/** Acknowledgement bytes */
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

#endif /*_LCOM_I8042_H_ */
