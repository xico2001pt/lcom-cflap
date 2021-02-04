#ifndef _KBC_H_
#define _KBC_H_

#include <stdbool.h>
#include <stdint.h>

/** @defgroup kbc kbc
 * @{
 *
 * KBC
 */

/**
 * Send cmd to the specified port
 * @param cmd Command byte
 * @param port Port
 * @return 0 on success, non-zero otherwise
 */
int kbc_issue_command(uint8_t cmd, int port);

/**
 * Read KBC's OUTB and return it through extern uint8_t out_byte
 * @return 0 on success, non-zero otherwise
 */
int kbc_read_outb();

#endif /* _KBC_H_ */
