/*
 * ds1307.h
 * Author: Kevin Zheng
 */

#ifndef INC_DS1307_H_
#define INC_DS1307_H_

#include "stm32f4xx_hal.h"

/*
 * I2C Address
 */

#define DS1307_I2C_ADDR (0x68 << 1)

/*
 * Register Addresses
 */

#define DS1307_REG_SECONDS	0x00
#define DS1307_REG_MINUTES	0x01
#define DS1307_REG_HOURS	0x02
#define DS1307_REG_DAY		0x03
#define DS1307_REG_DATE		0x04
#define DS1307_REG_MONTH	0x05
#define DS1307_REG_YEAR		0x06
#define DS1307_REG_CONTROL	0x07

/*
 * RAM Addresses
 */

#define DS1307_RAM_START	0x08
#define DS1307_RAM_END		0x3F

/*
 * Square Wave Output Frequency
 */
typedef enum {
	ONE_HZ = 0x00,
	FOUR_KHZ = 0x01,
	EIGHT_KHZ = 0x10,
	THIRTYTWO_KHZ = 0x11
} DS1307_SQW_FREQ;

/*
 * RTC Handle
 */

typedef struct {

	/* I2C handle */
	I2C_HandleTypeDef *i2cHandle;

	/* Year (0-99) */
	uint8_t year;

	/* Month (1-12) */
	uint8_t month;

	/* Date (1-31) */
	uint8_t date;

	/* Day (1-7) */
	uint8_t day;

	/* Hours (0-23) */
	uint8_t hours;

	/* Minutes (0-59) */
	uint8_t minutes;

	/* Seconds (0-59) */
	uint8_t seconds;

} DS1307;

/*
 * Initialization
 * The RTC is set to 24-hour mode on success.
 */

uint8_t DS1307_Initialize(DS1307* ds1307, I2C_HandleTypeDef* handle);

/*
 * Read Clock
 */

HAL_StatusTypeDef DS1307_ReadClock(DS1307* ds1307);

/*
 * Set Clock
 */

HAL_StatusTypeDef DS1307_WriteClock(DS1307* ds1307);

/*
 * Read RAM
 */

HAL_StatusTypeDef DS1307_ReadRam(DS1307* ds1307, uint8_t addr, const char* buffer, uint8_t len);

/*
 * Write RAM
 */

HAL_StatusTypeDef DS1307_WriteRam(DS1307* ds1307, uint8_t addr, const char* buffer, uint8_t len);

/*
 * Start/Stop Clock
 */

HAL_StatusTypeDef DS1307_StartClock(DS1307* ds1307);
HAL_StatusTypeDef DS1307_StopClock(DS1307* ds1307);

/*
 * Square Wave Control
 */

HAL_StatusTypeDef DS1307_SQW_Enable(DS1307* ds1307);
HAL_StatusTypeDef DS1307_SQW_Disable(DS1307* ds1307);
HAL_StatusTypeDef DS1307_SQW_RateSet(DS1307* ds1307, DS1307_SQW_FREQ freq);

/*
 * Low-Level Interface
 */

HAL_StatusTypeDef DS1307_ReadRegister(DS1307* ds1307, uint8_t reg, uint8_t* data);
HAL_StatusTypeDef DS1307_ReadRegisters(DS1307* ds1307, uint8_t reg, uint8_t* data, uint8_t len);
HAL_StatusTypeDef DS1307_WriteRegister(DS1307* ds1307, uint8_t reg, uint8_t data);

/*
 * Private Functions
 */

static uint8_t _bcd_to_bin(uint8_t bcd);
static uint8_t _bin_to_bcd(uint8_t bcd);
static uint8_t _set_24h_mode(DS1307* ds1307);
static HAL_StatusTypeDef _read_control_register(DS1307* ds1307, uint8_t* data);

#endif /* INC_DS1307_H_ */



















