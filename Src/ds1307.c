/*
 * ds1307.c
 * Author: Kevin Zheng
 */

#include "ds1307.h"

/*
 * Static function prototypes
 */

static uint8_t _bcd_to_bin(uint8_t bcd);
static uint8_t _bin_to_bcd(uint8_t decimal);
static uint8_t _set_24h_mode(const DS1307* ds1307);
static HAL_StatusTypeDef _read_control_register(const DS1307* ds1307, uint8_t* data);

/*
 * Initialization
 */

uint8_t DS1307_Initialize(DS1307* ds1307, I2C_HandleTypeDef* handle) {

    ds1307->year      = 0;
    ds1307->month     = 0;
    ds1307->date      = 0;
    ds1307->day       = 0;
    ds1307->hours     = 0;
    ds1307->minutes   = 0;
    ds1307->seconds   = 0;
    ds1307->i2cHandle = handle;

    /*
     * Verify connection to RTC by checking if day register value is 1-7
     * Then, set the RTC to use 24-hour mode
     */

    uint8_t day;
    if( DS1307_ReadRegister(ds1307, DS1307_REG_DAY, &day) != HAL_OK ) {
        return HAL_ERROR;
    } else if( day >= 1 && day <= 7 ) {
        return _set_24h_mode(ds1307);
    } else {
        return HAL_ERROR;
    }
}

/*
 * Read Clock
 */

HAL_StatusTypeDef DS1307_ReadClock(DS1307* ds1307) {

    uint8_t clock_data[DS1307_CLOCK_REG_LEN];
    if( DS1307_ReadRegisters(ds1307, DS1307_REG_SECONDS, clock_data, DS1307_CLOCK_REG_LEN) != HAL_OK ) {
        return HAL_ERROR;
    }

    ds1307->seconds = _bcd_to_bin(clock_data[0] & 0b01111111); /* Mask CH bit */
    ds1307->minutes = _bcd_to_bin(clock_data[1]);
    ds1307->hours   = _bcd_to_bin(clock_data[2]); /* No masks needed; we assume 24h mode initialized */
    ds1307->day     = clock_data[3];
    ds1307->date    = _bcd_to_bin(clock_data[4]);
    ds1307->month   = _bcd_to_bin(clock_data[5]);
    ds1307->year    = _bcd_to_bin(clock_data[6]);

    return HAL_OK;
}

/*
 * Set Clock
 */

HAL_StatusTypeDef DS1307_WriteClock(const DS1307* ds1307) {

    uint8_t clock_data[DS1307_CLOCK_REG_LEN];
    uint8_t seconds_reg; /* Get seconds register so that we can preserve state of CH bit */
    if( DS1307_ReadRegister(ds1307, DS1307_REG_SECONDS, &seconds_reg) != HAL_OK ) {
        return HAL_ERROR;
    }

    clock_data[0] = (seconds_reg & 0b10000000) | _bin_to_bcd(ds1307->seconds);
    clock_data[1] = _bin_to_bcd(ds1307->minutes);
    clock_data[2] = _bin_to_bcd(ds1307->hours);
    clock_data[3] = ds1307->day;
    clock_data[4] = _bin_to_bcd(ds1307->date);
    clock_data[5] = _bin_to_bcd(ds1307->month);
    clock_data[6] = _bin_to_bcd(ds1307->year);

    return DS1307_WriteRegisters(ds1307, DS1307_REG_SECONDS, clock_data, DS1307_CLOCK_REG_LEN);
}

/*
 * Clear the clock halt bit to run the clock
 */

HAL_StatusTypeDef DS1307_StartClock(const DS1307* ds1307) {

    uint8_t seconds_reg;
    if( DS1307_ReadRegister(ds1307, DS1307_REG_SECONDS, &seconds_reg) != HAL_OK ) {
        return HAL_ERROR;
    }

    uint8_t clock_halt_mask = (uint8_t) ~(0b10000000);
    return DS1307_WriteRegister( ds1307, DS1307_REG_SECONDS, seconds_reg & clock_halt_mask );
}

/*
 * Set the clock halt bit to pause the clock
 */

HAL_StatusTypeDef DS1307_StopClock(const DS1307* ds1307) {

    uint8_t seconds_reg;
    if( DS1307_ReadRegister(ds1307, DS1307_REG_SECONDS, &seconds_reg) != HAL_OK ) {
        return HAL_ERROR;
    }

    uint8_t clock_halt_mask = 0b10000000;
    return DS1307_WriteRegister( ds1307, DS1307_REG_SECONDS, seconds_reg | clock_halt_mask );
}

/*
 * Enable Square Wave Output
 */

HAL_StatusTypeDef DS1307_SQW_Enable(const DS1307* ds1307) {

    uint8_t control_reg_data;

    if( _read_control_register(ds1307, &control_reg_data) != HAL_OK ) {
        return HAL_ERROR;
    }

    uint8_t sqw_enable = 0b00010000 | control_reg_data; /* Disable the 4th bit */
    return DS1307_WriteRegister( ds1307, DS1307_REG_CONTROL, sqw_enable );
}

/*
 * Disable Square Wave Output
 */

HAL_StatusTypeDef DS1307_SQW_Disable(const DS1307* ds1307) {

    uint8_t control_reg_data;
    if( _read_control_register(ds1307, &control_reg_data) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t sqw_disable = 0b11101111 & control_reg_data;
    return DS1307_WriteRegister( ds1307, DS1307_REG_CONTROL, sqw_disable );
}

/*
 * Set frequency of Square Wave Output
 */

HAL_StatusTypeDef DS1307_SQW_RateSet(const DS1307* ds1307, DS1307_SQW_FREQ freq) {

    uint8_t control_reg_data;
    if( _read_control_register(ds1307, &control_reg_data) != HAL_OK) {
        return HAL_ERROR;
    }

    uint8_t freq_reg_value = freq | (control_reg_data & 0b11111100);
    return DS1307_WriteRegister( ds1307, DS1307_REG_CONTROL, freq_reg_value );
}

/*
 * Low-Level Interface
 */

HAL_StatusTypeDef DS1307_ReadRegister(const DS1307* ds1307, uint8_t reg, uint8_t* data) {
    return HAL_I2C_Mem_Read( ds1307->i2cHandle, DS1307_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY );
}

HAL_StatusTypeDef DS1307_ReadRegisters(const DS1307* ds1307, uint8_t reg, uint8_t* data, uint8_t len) {
    return HAL_I2C_Mem_Read( ds1307->i2cHandle, DS1307_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

HAL_StatusTypeDef DS1307_WriteRegister(const DS1307* ds1307, uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write( ds1307->i2cHandle, DS1307_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY );
}

HAL_StatusTypeDef DS1307_WriteRegisters(const DS1307* ds1307, uint8_t reg, uint8_t* data, uint8_t len) {
    return HAL_I2C_Mem_Write( ds1307->i2cHandle, DS1307_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY );
}

/*
 * Converts 8-bit binary-coded decimal number representation to binary
 */

static uint8_t _bcd_to_bin(uint8_t bcd) {

    uint8_t tens = (bcd >> 4);
    uint8_t ones = bcd & 0xF;

    return tens * 10 + ones;
}

/*
 * Converts decimal to 8-bit binary coded decimal number
 */

static uint8_t _bin_to_bcd(uint8_t decimal) {

    uint8_t tens = decimal / 10;
    uint8_t ones = decimal % 10;

    return (tens << 4) | ones;
}

/*
 * Sets DS1307 to 24-hour mode
 *
 * Returns 0 on success, 1 on failure
 */

static uint8_t _set_24h_mode(const DS1307* ds1307) {

    /*
     * Check if we are in 24 hour mode
     * 12/24-hour mode bit is 6th bit in DS1307_REG_HOURS
     * When high, 12-hour mode selected
     */
    uint8_t hour_reg;
    if( DS1307_ReadRegister(ds1307, DS1307_REG_HOURS, &hour_reg) != HAL_OK ) {
        return HAL_ERROR;
    }
    if( !(0b01000000 & hour_reg) ) {
        return HAL_OK;
    }

    /*
     * We need to change to 24h mode
     * Changing mode bit requires us to re-enter hours value
     */
    uint8_t am_pm_mask = 0b00100000;
    uint8_t twelve_hrs_mask = 0b00011111;
    uint8_t hours = _bcd_to_bin(hour_reg & twelve_hrs_mask);

    /* Convert hour from 12-hour format to 24-hour format */
    if((hour_reg & am_pm_mask) != 0 && hours < 12) { // PM
        hours += 12;
    } else if ((hour_reg & am_pm_mask) == 0 && hours == 12) { // Special case: 12 AM
        hours = 0;
    }

    uint8_t bcd_hours = _bin_to_bcd(hours); /* Will have 12/24h mode bit low for 24h mode */

    return DS1307_WriteRegister(ds1307, DS1307_REG_HOURS, bcd_hours);
}

/*
 * Reads the value of the control register and places it in data argument
 */

static HAL_StatusTypeDef _read_control_register(const DS1307* ds1307, uint8_t* data) {
    return DS1307_ReadRegister(ds1307, DS1307_REG_CONTROL, data);
}
