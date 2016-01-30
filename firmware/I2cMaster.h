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
#ifndef I2cMaster_h
#define I2cMaster_h
#include "application.h"
#include "i2c_lld.h"
#include "WireMaster.h"
/**
 * @class I2cMaster
 * @brief I2C polled master class.
 */
class I2cMaster {
 public:
  /** Create an I2cMaster object for a specified I2C interface.
   *
   * @param[in] i2cIf I2C interface.
   */
  explicit I2cMaster(HAL_I2C_Interface i2cIf = HAL_I2C_INTERFACE1)
           : m_rtn(0), m_i2cIf(i2cIf) {}

  /** Initialize the I2cMaster interface.
   *
   * @param hz The bus frequency in hertz
   *
   * @returns true for success else false.
   */
  bool begin(uint32_t hz = 100000);

  /** Disable the I2C interface.
   *
   * @returns true for success else false.
   */
  bool end();

  /** Set scl frequency.
   *
   * @param[in] hz The bus frequency in Hz.
   *
   * @returns true for success else false.
   */
  bool frequency(uint32_t hz);
  
  /** Read from an I2C slave
   *
   * @param[in] address Right justified 7-bit address.
   * @param[out] buf Buffer for read data.
   * @param[in] count Number of bytes to read.
   * @param[in] stop Generate stop if true.
   *
   * @returns true for success else false.
   */
  bool read(uint8_t address, void* buf, size_t count, bool stop = true);

  /** Return low level driver info.
   *
   * @returns See low level driver.
   */
  int rtn() {return m_rtn;}

  /** Creates a stop condition.
   *
   * @returns true for success else false.
   */
  bool stop();
  
  /** Write single byte to a selected slave.
   *
   * @param[in] data data to write out on bus
   *
   * @returns true for success else false.
   * @param[in] stop Generate stop if true.   
   */
  bool write(uint8_t data, bool stop);
  
  /** Write to a selected slave.
   *
   * Continue after a write without a stop.
   *
   * @param[in] buf Data to send.
   * @param[in] count Number of bytes to send.
   * @param[in] stop Generate stop if true.
   *
   * @returns true for success else false.
   */  
  bool write(const void* buf, size_t count, bool stop);
  
  /** Write to an I2C slave
   *
   * @param[in] address Right justified 7-bit address.
   * @param[in] buf Data to send.
   * @param[in] count Number of bytes to send.
   * @param[in] stop Generate stop if true.
   *
   * @returns true for success else false.
   */
  bool write(uint8_t address, const void* buf, size_t count, bool stop = true);

 private:
  int m_rtn;
  HAL_I2C_Interface m_i2cIf;
};
#endif  // I2cMaster_h