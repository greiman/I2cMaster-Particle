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
/* Arduino Wire Master functionality. */
#include "WireMaster.h"
#include "i2c_lld.h"

WireMaster::WireMaster(HAL_I2C_Interface i2cIf) {
  m_frequency = 100000;
  m_i2cIf = i2cIf;
  m_rxBufferIndex = 0;
  m_rxBufferLength = 0;
  m_txBufferLength = 0;
  m_transmitting = 0;
}

int WireMaster::available() {
  return m_rxBufferLength - m_rxBufferIndex;
}

void WireMaster::begin() {
  m_rxBufferIndex = 0;
  m_rxBufferLength = 0;
  m_txBufferLength = 0;
  m_transmitting = 0;
  i2c_begin(m_i2cIf, m_frequency);
}

void WireMaster::beginTransmission(uint8_t address) {
  m_txAddress = address;
  m_transmitting = 1; 
  m_txBufferLength = 0;
}

void WireMaster::beginTransmission(int address) {
  beginTransmission((uint8_t)address);
}

void WireMaster::end() {
  i2c_end(m_i2cIf);
}

uint8_t WireMaster::endTransmission(uint8_t stop) {
  uint8_t rtn = 1;
  if (m_transmitting) {
    m_rtn = i2c_write(m_i2cIf, m_txAddress, m_txBuffer, m_txBufferLength, stop);
    rtn = m_rtn < 0 ? 2 : 0;
  }
  m_txBufferLength = 0;
  m_transmitting = 0;  
  return rtn;
}

uint8_t WireMaster::endTransmission() {
  return endTransmission(true);
}

void WireMaster::flush() {
  // to be implemented.
}

int WireMaster::peek() {
  if (m_rxBufferIndex < m_rxBufferLength){
    return m_rxBuffer[m_rxBufferIndex];
  }
  return -1;
}

int WireMaster::read() {
  if (m_rxBufferIndex < m_rxBufferLength){
    return m_rxBuffer[m_rxBufferIndex++];
  }
  return -1;
}

size_t WireMaster::requestFrom(uint8_t address, size_t quantity, uint8_t sendStop) {
  // Follow Arduino if quantity too large.
  if (quantity > WIRE_MASTER_BUFFER_LENGTH) {
    quantity = WIRE_MASTER_BUFFER_LENGTH;
  }
  m_rtn = i2c_read(m_i2cIf, address, m_rxBuffer, quantity, sendStop);
  m_rxBufferIndex = 0;
  m_rxBufferLength = m_rtn < 0 ? 0 : quantity;
  return m_rxBufferLength;
}
size_t WireMaster::requestFrom(uint8_t address, size_t quantity) {
  return requestFrom(address, quantity, 1);
}
  
void WireMaster::setClock(uint32_t frequency) {
  m_frequency = frequency;
  i2c_frequency(m_i2cIf, m_frequency);
}

size_t WireMaster::write(uint8_t data) {
  if (m_transmitting && m_txBufferLength < WIRE_MASTER_BUFFER_LENGTH){
    m_txBuffer[m_txBufferLength++] = data;    
    return 1;
  }
  // Error so don't send data.
  m_transmitting = 0;
  return 0;
}

size_t WireMaster::write(const uint8_t *data, size_t quantity) {
  size_t n;
  for (n = 0; n < quantity; n++){
    if (1 != write(data[n])) {
        break;
    }
  }
  return n;
}
