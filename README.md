# ds1307_stm32
DS1307 Real-Time Clock driver for STM32

![A diagram showing an example application circuit with the DS1307](https://github.com/jx-zheng/ds1307_stm32/assets/43159579/e8699f93-a013-422d-a9e5-86b10e564188)

## Introduction

This repository contains a driver for the popular DS1307 real-time clock by Maxim Integrated, written in C. From the DS1307 [datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/DS1307.pdf):

> The DS1307 serial real-time clock (RTC) is a lowpower, full binary-coded decimal (BCD) clock/calendar plus 56 bytes of NV SRAM. Address and data are transferred serially through an I2C, bidirectional bus.
The clock/calendar provides seconds, minutes, hours, day, date, month, and year information. The end of the month date is automatically adjusted for months with fewer than 31 days, including corrections for leap year. 

The driver implements allows for easy use of all essential DS1307 functions, including setting and retrieving time and date, configuring square wave output, and writing and reading from the RAM registers.

## Compatibility

This driver works with any STM32F4 (Arm Cortex M4) out-of-the-box, and was tested on an STM32 Black Pill board (STM32F401RE). Making this driver compatible with other STM32Fx-series MCUs can be achieved by simply changing the included HAL support header in Inc/ds1307.h.

## Files and Usage

Add Inc/ds1307.h and Src/ds1307.c to your STM32 project in the corresponding folders. main.c provides a usage example of the driver on an STM32F4xx MCU,
where UART serial output is set up to initialize the driver, set a time and date, and write the time and date to the serial port every 5 seconds. Note that main.c will not build on its own; it depends on MCU-specific code generated via STM32CubeMX.

![DS1307 usage and serial output demo](https://github.com/jx-zheng/ds1307_stm32/assets/43159579/83fb33cc-1b03-4c7b-875e-07507ed8da30)

