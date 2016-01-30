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
#include "application.h"
#include "I2cMaster.h"

bool I2cMaster::begin(uint32_t hz) {
  m_rtn = i2c_begin(m_i2cIf, hz);
  return m_rtn >= 0;
}

bool I2cMaster::end() {
  m_rtn = i2c_end(m_i2cIf);
  return m_rtn >= 0;
}

bool I2cMaster::frequency(uint32_t hz) {
  m_rtn = i2c_frequency(m_i2cIf, hz);
  return m_rtn >= 0;
}

bool I2cMaster::read(uint8_t address, void* buf, size_t count, bool stop) {
  m_rtn = i2c_read(m_i2cIf, address, buf, count, stop);
  return m_rtn >= 0;
}

bool I2cMaster::stop() {
  m_rtn =  i2c_stop(m_i2cIf);
  return m_rtn >= 0;  
}

bool I2cMaster::write(uint8_t data, bool stop) {
  m_rtn =  i2c_write_data(m_i2cIf, &data, 1, stop);
  return m_rtn >= 0;
}

bool I2cMaster::write(const void* buf, size_t count, bool stop) {
  m_rtn =  i2c_write_data(m_i2cIf, buf, count, stop);
  return m_rtn >= 0;
}

bool I2cMaster::write(uint8_t address, const void* buf, size_t count, bool stop) {
  m_rtn = i2c_write(m_i2cIf, address, buf, count, stop);
  return m_rtn >= 0;
}