#ifndef __RTC_H__
#define __RTC_H__
#include <stdint.h>

/** @defgroup rtc rtc
 * @{
 *
 * RTC
 */

#define RTC_ADDR_REG 0x70
#define RTC_DATA_REG 0x71
#define RTC_IRQ 8

#define RTC_SECONDS_ADDRESS 0
#define RTC_SECONDS_ALARM_ADDRESS 1
#define RTC_MINUTES_ADDRESS 2
#define RTC_MINUTES_ALARM_ADDRESS 3
#define RTC_HOURS_ADDRESS 4
#define RTC_HOURS_ALARM_ADDRESS 5
#define RTC_DAY_OF_WEEK_ADDRESS 6
#define RTC_DAY_OF_MONTH_ADDRESS 7
#define RTC_MONTH_ADDRESS 8
#define RTC_YEAR_ADDRESS 9
#define RTC_REGISTER_A 10
#define RTC_REGISTER_B 11
#define RTC_REGISTER_C 12
#define RTC_REGISTER_D 13

#define RTC_DONT_CARE_VALUE BIT(7) | BIT(6)

#define RTC_UF BIT(4)
#define RTC_AF BIT(5)
#define RTC_PF BIT(6)

/**
 * Initializes RTC
 * @return 0 on success, non-zero otherwise
 */
int rtc_init();

/**
 * Reset RTC to it's initial state
 * @return 0 on success, non-zero otherwise
 */
int rtc_cleanup();
/**
 * Sets RTC alarm
 * @param hour hour
 * @param minute minute
 * @param second second
 * @return 0 on success, non-zero otherwise
 */
int rtc_set_alarm(uint8_t hour, uint8_t minute, uint8_t second);
/**
 * Subscribes RTC interrupts
 * @param bit_no bit number of the device
 * @return 0 on success, non-zero otherwise
 */
int rtc_subscribe_int(uint8_t *bit_no);
/**
 * Unsubscribes RTC interrupts
 * @return 0 on success, non-zero otherwise
 */
int rtc_unsubscribe_int();
/**
 * Reads RTC register
 * @param address address to be read
 * @param data data returned as a parameter
 * @return 0 on success, non-zero otherwise
 */
int rtc_read_register(uint8_t address, uint8_t *data);
/**
 * Writes RTC register
 * @param address address to be read
 * @param data data to be written
 * @return 0 on success, non-zero otherwise
 */
int rtc_set_register(uint8_t address, uint8_t data);
/**
 * Reads RTC date to the string
 * @param string string returned as a parameter
 * @return 0 on success, non-zero otherwise
 */
int rtc_read_date_to_string(char *string);
/**
 * Reads RTC time to the string
 * @param string string returned as a parameter
 * @return 0 on success, non-zero otherwise
 */
int rtc_read_time_to_string(char *string);

#endif
