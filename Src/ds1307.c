/*
 * ds1307.c
 * Author: Kevin Zheng
 */

#include "ds1307.h"

/*
 * Initialization
 */

uint8_t DS1307_Initialize(DS1307* ds1307, I2C_HandleTypeDef* handle) {

	ds1307->year 		= 0;
	ds1307->month 		= 0;
	ds1307->date 		= 0;
	ds1307->hours 		= 0;
	ds1307->minutes 	= 0;
	ds1307->seconds 	= 0;
	ds1307->i2cHandle 	= handle;

	/*
	 * Verify connection to RTC by checking if day register value is 1-7
	 * Then, set the RTC to use 24-hour mode
	 */

	uint8_t day;
	if( DS1307_ReadRegister(ds1307, DS1307_REG_DAY, &day) != HAL_OK ) {
		return 1;
	} else if( day >= 1 && day <= 7 ) {
		return _set_24h_mode(ds1307);
	} else {
		return 1;
	}

}

/*
 * Read Clock
 */

HAL_StatusTypeDef DS1307_ReadClock(DS1307* ds1307) {

}

/*
 * Set Clock
 */

HAL_StatusTypeDef DS1307_WriteClock(DS1307* ds1307) {

}

/*
 * Low-Level Interface
 */

HAL_StatusTypeDef DS1307_ReadRegister(DS1307* ds1307, uint8_t reg, uint8_t* data) {

	return HAL_I2C_Mem_Read( ds1307->i2cHandle, DS1307_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY );

}

HAL_StatusTypeDef DS1307_ReadRegisters(DS1307* ds1307, uint8_t reg, uint8_t* data, uint8_t len) {

	return HAL_I2C_Mem_Read( ds1307->i2cHandle, DS1307_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);

}

HAL_StatusTypeDef DS1307_WriteRegister(DS1307* ds1307, uint8_t reg, uint8_t data) {

	return HAL_I2C_Mem_Write( ds1307->i2cHandle, DS1307_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY );

}

/*
 * Helper function to convert 8-bit binary-coded decimal number representation to binary
 */

static uint8_t _bcd_to_bin(uint8_t bcd) {

	uint8_t tens = (bcd >> 4);

	return 0;

}

/*
 * Helper function to convert decimal to 8-bit binary coded decimal number
 */

static uint8_t _bin_to_bcd(uint8_t bcd) {

	uint8_t tens = (bcd >> 4);

	return 0;

}

/*
 * Helper function to set DS1307 to 24-hour mode
 *
 * Returns 0 on success, 1 on failure
 */

static uint8_t _set_24h_mode(DS1307* ds1307) {

	/*
	 * Check if we are in 24 hour mode
	 * 12/24-hour mode bit is 6th bit in DS1307_REG_HOURS
	 * When high, 12-hour mode selected
	 */

	uint8_t hour_reg;
	if( DS1307_ReadRegister(ds1307, DS1307_REG_HOURS, &hour_reg) != HAL_OK ) {
		return 1;
	}
	uint8_t mask = 0b01000000;
	if( !(mask & hour_reg) ) {
		/* We are already in 24 hour mode */
		return 0;
	}

	/*
	 * We need to change to 24h mode
	 * Changing mode bit requires us to re-enter hours value
	 */

	if() {
		return 0;
	}


}
