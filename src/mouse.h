#ifndef __MOUSE_H_
#define __MOUSE_H_

/** @defgroup mouse mouse
 * @{
 *
 * Mouse
 */

/**
 * Subscribe mouse interrupts
 * @param bit_no bit number of the device
 * @return 0 on success, non-zero otherwise
 */
int mouse_subscribe_int(uint8_t *bit_no);

/**
 * Unsubscribe mouse interrupts
 * @return 0 on success, non-zero otherwise
 */
int mouse_unsubscribe_int();

/**
 * Enable mouse reporting data
 * @return 0 on success, non-zero otherwise
 */
int mouse_enable_reporting();

/**
 * Disable mouse reporting data
 * @return 0 on success, non-zero otherwise
 */
int mouse_disable_reporting();

/**
 * Read mouse data
 * @return 0 on success, non-zero otherwise
 */
int read_data_remote();

/**
 * Set mouse streaming mode
 * @return 0 on success, non-zero otherwise
 */
int mouse_set_streaming_mode();

/**
 * Process mouse packets
 * @param bytes bytes to be processed
 * @return processed packet
 */
struct packet process_packets(uint8_t *bytes);

/**
 * Converts to cpl2 if MSB
 * @param byte byte to be converted
 * @param MSB indicates of MSB
 * @return converted number
 */
uint16_t cpl2_delta(uint8_t byte, bool MSB);

#define MOUSE_LB BIT(0)
#define MOUSE_RB BIT(1)
#define MOUSE_MB BIT(2)
#define MOUSE_MSB_X BIT(4)
#define MOUSE_MSB_Y BIT(5)
#define MOUSE_X_OVFL BIT(6)
#define MOUSE_Y_OVFL BIT(7)

#endif
