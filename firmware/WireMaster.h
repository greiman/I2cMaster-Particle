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

class WireMaster : public Stream {
 public:
  explicit WireMaster(HAL_I2C_Interface i2cIf = HAL_I2C_INTERFACE1);
  void begin();
  void beginTransmission(uint8_t);
  void beginTransmission(int);
  void end();  
  size_t endTransmission(void);
  size_t endTransmission(uint8_t);
  size_t requestFrom(uint8_t, size_t);
  size_t requestFrom(uint8_t, size_t, uint8_t);
  void setClock(uint32_t);
  
  virtual int available(void);
  virtual void flush(void);  
  virtual int peek(void);
  virtual int read(void);
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t*, size_t);
  
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }  
  
  using Print::write;
  
 private:
  HAL_I2C_Interface m_i2cIf;
  uint8_t m_rxBuffer[WIRE_MASTER_BUFFER_LENGTH];
  size_t m_rxBufferIndex;
  size_t m_rxBufferLength;
  uint8_t m_txAddress;
  uint8_t m_txBuffer[WIRE_MASTER_BUFFER_LENGTH];
  size_t m_txBufferLength;

  uint8_t m_transmitting;  
};

#endif  // WireMaster_h

