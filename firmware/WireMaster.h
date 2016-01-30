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
/*
  Arduino Wire Master functionality.
*/
#ifndef WireMaster_h
#define WireMaster_h

#include "application.h"

#define WIRE_MASTER_BUFFER_LENGTH 32

// WIRE_HAS_END means Wire has end()
#define WIRE_HAS_END 1
/**
 * @class WireMaster
 * @brief Arduino Wire style class.
 */
class WireMaster : public Stream {
 public:
  /** Create an I2cMaster object for a specified I2C interface.
   *
   * @param[in] i2cIf I2C interface.
   */
  explicit WireMaster(HAL_I2C_Interface i2cIf = HAL_I2C_INTERFACE1);
  
  /**
   * @return Returns the number of bytes available for retrieval with read().
   */   
  virtual int available();
  
  /** Initialize the I2C interface.
   *
   */  
  void begin();
  /** Begin a transmission to the I2C slave device with the given address.
   *
   * @param[in] address Right justified 7-bit address.
   */
  void beginTransmission(uint8_t address);
  
  /** Begin a transmission to the I2C slave device with the given address.
   *
   * @param[in] address Right justified 7-bit address.
   */  
  void beginTransmission(int address);
  
  /** Disable the I2C interface. */  
  void end();
  
  /** Ends a transmission to a slave device that was begun by beginTransmission().
   * Transmits the bytes that were queued by write(). 
   *
   * @return zero for success else error code.
   */
  uint8_t endTransmission();
  
  /** Ends a transmission to a slave device that was begun by beginTransmission().
   * Transmits the bytes that were queued by write(). 
   *
   * @param[in] stop Generate stop if true.
   *
   * @return zero for success else error code.
   */  
  uint8_t endTransmission(uint8_t stop);
 
  /** Not implemented */
  virtual void flush();  
  
  /** Returns the next byte without removing it from the serial buffer.
   *
   * @return -1 if no data is available else the byte.   
   */
  virtual int peek();
  
  /** Reads a byte that was transmitted from a slave device
   *
   * @return Returns -1 if no data is available else the byte.
   */
  virtual int read();
   
  /** Request bytes from a slave device.
   *
   * @param[in] address Right justified 7-bit address.
   * @param[in] quantity Number of bytes to read.
   *
   * @returns The number of bytes returned from the slave device.
   */
  size_t requestFrom(uint8_t address, size_t quantity);
  
  /** Request bytes from a slave device.
   *
   * @param[in] address Right justified 7-bit address.
   * @param[in] quantity Number of bytes to read.
   * @param[in] stop Generate stop if true.
   *
   * @returns The number of bytes returned from the slave device.
   */
  size_t requestFrom(uint8_t address, size_t quantity, uint8_t stop);
  
  /** Return low level driver info.
   *
   * @returns See low level driver.
   */
  int rtn() {return m_rtn;}
  
  /** Set scl frequency.
   *
   * @note May be called before or after after begin().
   *
   * @param[in] hz The bus frequency in Hz.
   */  
  void setClock(uint32_t hz);
  
  /** Set scl frequency.
   *
   * @note May be called before or after after begin().
   *
   * @param[in] hz The bus frequency in Hz.
   */  
  void setSpeed(uint32_t hz) {setClock(hz);}
  
  /** Queues a byte for transmission from a master to slave device.
   *
   * @param[in] data The byte to be queued.
   *
   * @return One if the byte is queued else zero.
   */
  virtual size_t write(uint8_t data);
  
  /** Queues bytes for transmission from a master to slave device.
   *
   * @param[in] buf Location of data to be queued.
   * @param[in] quantity Number of bytes to queue.
   *
   * @return Number of bytes queued.
   */  
  virtual size_t write(const uint8_t* buf, size_t quantity);

  /** Queues a byte for transmission from a master to slave device.
   *
   * @param[in] data The byte to be queued.
   *
   * @return One if the byte is queued else zero.
   */  
  inline size_t write(unsigned long data) {return write((uint8_t)data);}
  
  /** Queues a byte for transmission from a master to slave device.
   *
   * @param[in] data The byte to be queued.
   *
   * @return One if the byte is queued else zero.
   */  
  inline size_t write(long data) {return write((uint8_t)data);}
  
  /** Queues a byte for transmission from a master to slave device.
   *
   * @param[in] data The byte to be queued.
   *
   * @return One if the byte is queued else zero.
   */  
  inline size_t write(unsigned int data) {return write((uint8_t)data);}
  
  /** Queues a byte for transmission from a master to slave device.
   *
   * @param[in] data The byte to be queued.
   *
   * @return One if the byte is queued else zero.
   */  
  inline size_t write(int data) {return write((uint8_t)data);}  
  
  using Print::write;
  
 private:
  uint32_t m_frequency;
  HAL_I2C_Interface m_i2cIf;
  int m_rtn;
  uint8_t m_rxBuffer[WIRE_MASTER_BUFFER_LENGTH];
  size_t m_rxBufferIndex;
  size_t m_rxBufferLength;
  uint8_t m_txAddress;
  uint8_t m_txBuffer[WIRE_MASTER_BUFFER_LENGTH];
  size_t m_txBufferLength;
  uint8_t m_transmitting;  
};

#endif  // WireMaster_h

