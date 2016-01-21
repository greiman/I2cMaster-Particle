/* Particle I2cMaster Library
 * Copyright (C) 2016 by William Greiman
 *
 * This file is part of the Particle I2cMaster Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Particle I2cMaster Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef i2c_lld_h
#define i2c_lld_h
#include "i2c_hal.h"
#include "gpio_hal.h"
#include "timer_hal.h"
#include "pinmap_impl.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize the I2C peripheral. It sets the default parameters for I2C
 * peripheral, and configures its specifieds pins.
 *
 * @param[in] i2cIf The I2C interface.
 * @param[in] hz Frequency in Hz.
 *
 * @return
 */
int i2c_begin(HAL_I2C_Interface i2cIf, uint32_t hz);


int i2c_end(HAL_I2C_Interface i2cIf);

/** Set the I2C frequency.
 *
 * @param[in] i2cIf The I2C interface.
 * @param[in] hz Frequency in Hz.
 *
 * @return
 */
int i2c_frequency(HAL_I2C_Interface i2cIf, uint32_t hz);

/** Genetate a stop condition.
 *
 * @param[in] i2cIf The I2C interface.
 *
 * @return Error if less than zero else success.
 */
int  i2c_stop(HAL_I2C_Interface i2cIf);

/** Blocking reading data
 *
 * @param[in] i2cIf The I2C interface.
 * @param[in] address Right justified 7-bit address.
 * @param[out] buf The buffer for receiving
 * @param[in] count Number of bytes to read
 * @param[in] stop If non-zero, generated after the transfer is done.
 *
 * @return Error if less than zero else the number of bytes read.
 */
int i2c_read(HAL_I2C_Interface i2cIf, uint8_t address, void *buf, size_t count, int stop);

/** Write with start.
 *
 * @param[in] i2cIf The I2C interface
 * @param[in] address Right justified 7-bit address.
 * @param[in] buf The buffer for sending.
 * @param[in] count Number of bytes to write.
 * @param[in] stop If non-zero, generate stop condition.
 *
 * @return Error if less than zero else the number of bytes written.
 */
int i2c_write(HAL_I2C_Interface i2cIf, uint8_t address, const void *buf, size_t count, int stop);

/** Write data after write with start.
 *
 * @param[in] i2cIf  The I2C interface
 * @param[in] buf data to be written
 * @param[in] count Number of bytes to write
 * @param[in] stop If non-zero, generate stop condition.
 
 * @return Error if less than zero else success.
 */
int i2c_write_data(HAL_I2C_Interface i2cIf, const void* buf, size_t count, int stop);
#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // i2c_lld_h

