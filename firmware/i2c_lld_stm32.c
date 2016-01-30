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
#include "i2c_lld.h"

#define I2C_ERROR_ARG         (-(10000 + __LINE__))
#define I2C_ERROR_TIMEOUT     (-(20000 + __LINE__))
#define I2C_ERROR_ACK_FAILURE (-(30000 + __LINE__))

/* Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will
   not remain stuck if the I2C communication is corrupted. */
#define FLAG_TIMEOUT ((int)0x1000)
#define LONG_TIMEOUT ((int)0x8000)
//-----------------------------------------------------------------------------
typedef struct STM32_I2C_Info {
  I2C_TypeDef*   i2c;
  __IO uint32_t* rccEnbReg;
  uint32_t       rccEnbBit;
  uint16_t       sdaPin;
  uint16_t       sclPin;
  uint8_t        pinAf;
} STM32_I2C_Info;

/*
 * I2C mapping
 */
//-----------------------------------------------------------------------------
static STM32_I2C_Info I2C_MAP[] = {
#if PLATFORM_ID < 3
  // Core
  {I2C1, &RCC->APB1ENR, RCC_APB1Periph_I2C1, D0, D1, 0}
#else  // PLATFORM_ID < 3
  // Photon or Electron
  {I2C1, &RCC->APB1ENR, RCC_APB1Periph_I2C1, D0, D1, GPIO_AF_I2C1}
#endif  // PLATFORM_ID < 3
#if PLATFORM_ID == 10
  // Electron
  ,{I2C1, &RCC->APB1ENR, RCC_APB1Periph_I2C1, C4, C5, GPIO_AF_I2C1}
#if defined(PM_SDA_UC) && defined(PM_SCL_UC)
  // Probably won't be supported in released Electron.
  ,{I2C3, &RCC->APB1ENR, RCC_APB1Periph_I2C3,
    PM_SDA_UC, PM_SCL_UC, GPIO_AF_I2C3}
#endif  // defined(PM_SDA_UC) && defined(PM_SCL_UC)
#endif  // #if PLATFORM_ID < 3
};
//-----------------------------------------------------------------------------
#define N_I2C_IF  (sizeof(I2C_MAP)/sizeof(STM32_I2C_Info))
//-----------------------------------------------------------------------------
// Clear ADDR by reading SR1 register followed by reading SR2.
static void clearAddrFlag(I2C_TypeDef* i2c) {
  __IO uint32_t tmpreg = 0x00;
  tmpreg = i2c->SR1;
  tmpreg = i2c->SR2;
  (void)tmpreg;
}

static int waitForStopCondition(I2C_TypeDef* i2c) {
  int timeout = FLAG_TIMEOUT;
  while (i2c->CR1 & I2C_CR1_STOP) {
    if (--timeout <= 0) return 0;
  }
  return 1;
}

static int waitUntilBitSetSR1(I2C_TypeDef* i2c, uint32_t bit) {
  int timeout = FLAG_TIMEOUT;
  do {
    if (i2c->SR1 & bit) return 1;
  } while (timeout--);
  return 0;
}

static int waitUntilNotBusy(I2C_TypeDef* i2c) {
   int timeout = LONG_TIMEOUT;
  while (i2c->SR2 & I2C_SR2_BUSY) {
    if (--timeout <= 0) return 0;
  }
  return 1; 
}
//-----------------------------------------------------------------------------
int i2c_begin(HAL_I2C_Interface i2cIf, uint32_t hz) {
  if (i2cIf >= N_I2C_IF) {
    return I2C_ERROR_ARG;
  }
  STM32_I2C_Info* p = &I2C_MAP[i2cIf];

  /* Enable I2C clock */
  *p->rccEnbReg |= p->rccEnbBit;

  /* Enable and Release I2C Reset State */
  I2C_DeInit(p->i2c);

#if PLATFORM_ID > 3
  /* Connect I2C pins to respective AF */
  STM32_Pin_Info* PIN_MAP = HAL_Pin_Map();

  GPIO_PinAFConfig(PIN_MAP[p->sclPin].gpio_peripheral,
    PIN_MAP[p->sclPin].gpio_pin_source, p->pinAf);

  GPIO_PinAFConfig(PIN_MAP[p->sdaPin].gpio_peripheral,
    PIN_MAP[p->sdaPin].gpio_pin_source, p->pinAf);
#endif  // PLATFORM_ID > 3

  HAL_Pin_Mode(p->sclPin, AF_OUTPUT_DRAIN);
  HAL_Pin_Mode(p->sdaPin, AF_OUTPUT_DRAIN);

   return i2c_frequency(i2cIf, hz);
}
//-----------------------------------------------------------------------------
int i2c_end(HAL_I2C_Interface i2cIf) {
   if (i2cIf >= N_I2C_IF) {
    return I2C_ERROR_ARG;
  }
  STM32_I2C_Info* p = &I2C_MAP[i2cIf];

  waitUntilNotBusy(p->i2c);

  I2C_Cmd(p->i2c, DISABLE);

  return 0;
}
//-----------------------------------------------------------------------------
int i2c_frequency(HAL_I2C_Interface i2cIf, uint32_t hz) {
  if (i2cIf >= N_I2C_IF) {
    return I2C_ERROR_ARG;
  }
  STM32_I2C_Info* p = &I2C_MAP[i2cIf];

  if (hz > 400000) {
    return I2C_ERROR_ARG;
  }
  // wait before init
  waitUntilNotBusy(p->i2c);

  // I2C configuration
  I2C_InitTypeDef I2C_InitStructure;
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = hz;

  /* Apply I2C configuration */
  I2C_Init(p->i2c, &I2C_InitStructure);

  return 0;
}
//-----------------------------------------------------------------------------
int i2c_read(HAL_I2C_Interface i2cIf,
             uint8_t address, void *dst, size_t count, int stop) {
  if (i2cIf >= N_I2C_IF || count == 0) {
    return I2C_ERROR_ARG;
  }              
  I2C_TypeDef* pI2c = I2C_MAP[i2cIf].i2c;                 
  uint8_t *pData = (uint8_t*)dst;

  /* Disable Pos */
  pI2c->CR1 &= ~I2C_CR1_POS;


  /* Enable Acknowledge */
  pI2c->CR1 |= I2C_CR1_ACK;

  /* Generate Start */
  pI2c->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if (!waitUntilBitSetSR1(pI2c, I2C_SR1_SB)) {
   
    return I2C_ERROR_TIMEOUT;
  }

  /* Send slave address */
   pI2c->DR = (address << 1) | 1;

  /* Wait until ADDR flag is set */
  if (!waitUntilBitSetSR1(pI2c, I2C_SR1_ADDR)) {    
      return I2C_ERROR_TIMEOUT;
  }
  
  if (count == 1) {
    /* Disable Acknowledge */
    pI2c->CR1 &= ~I2C_CR1_ACK;

    /* Clear ADDR flag */
    clearAddrFlag(pI2c);

    /* Generate Stop */
    if (stop) {
      pI2c->CR1 |= I2C_CR1_STOP;
    }    
    /* Wait until RXNE flag is set */
    if (!waitUntilBitSetSR1(pI2c, I2C_SR1_RXNE)) {
      return I2C_ERROR_TIMEOUT;
    }

    /* Read data from DR */
    *pData++ = pI2c->DR;   
  } else if (count == 2) {
    /* Disable Acknowledge */
    pI2c->CR1 &= ~I2C_CR1_ACK;

    /* Enable Pos */
    pI2c->CR1 |= I2C_CR1_POS;

    /* Clear ADDR flag */
    clearAddrFlag(pI2c);
    
    /* Wait until BTF flag is set */
    if (!waitUntilBitSetSR1(pI2c, I2C_SR1_BTF)) {
      return I2C_ERROR_TIMEOUT;
    }

    /* Generate Stop */
    if (stop) {
      pI2c->CR1 |= I2C_CR1_STOP;
    }

    /* Read data from DR */
    *pData++ = pI2c->DR;

    /* Read data from DR */
    *pData++ = pI2c->DR;  
  } else {
    /* Enable Acknowledge */
    pI2c->CR1 |= I2C_CR1_ACK;

    /* Clear ADDR flag */
    clearAddrFlag(pI2c);
    
    int todo;    
    for (todo = count; todo > 3; todo--) {
      /* Wait until RXNE flag is set */
      if (!waitUntilBitSetSR1(pI2c, I2C_SR1_RXNE)) {
        return I2C_ERROR_TIMEOUT;
      }

      /* Read data from DR */
      *pData++ = pI2c->DR;   
    }
    /* 3 Last bytes */
    /* Wait until BTF flag is set */
    if (!waitUntilBitSetSR1(pI2c, I2C_SR1_BTF)) {
      return I2C_ERROR_TIMEOUT;
    }

    /* Disable Acknowledge */
    pI2c->CR1 &= ~I2C_CR1_ACK;

    /* Read data from DR */
    *pData++ = pI2c->DR;

    /* Wait until BTF flag is set */
    if (!waitUntilBitSetSR1(pI2c, I2C_SR1_BTF)) {
      return I2C_ERROR_TIMEOUT;
    }

    /* Generate Stop */
    if (stop) {
      pI2c->CR1 |= I2C_CR1_STOP;
    }

    /* Read data from DR */
    *pData++ = pI2c->DR;

    /* Read data from DR */
    *pData++ = pI2c->DR;  
  }
  if (stop && !waitForStopCondition(pI2c)) {
    return I2C_ERROR_TIMEOUT;
  }
  return count;
}
//-----------------------------------------------------------------------------
int i2c_stop(HAL_I2C_Interface i2cIf) {
  if (i2cIf >= N_I2C_IF) {
    return I2C_ERROR_ARG;
  }
  I2C_TypeDef* pI2c = I2C_MAP[i2cIf].i2c;
  
  pI2c->CR1 |= I2C_CR1_STOP;
  
  if (!waitForStopCondition(pI2c)) {
    return I2C_ERROR_TIMEOUT;
  }
  return 0;
}
//-----------------------------------------------------------------------------
int i2c_write(HAL_I2C_Interface i2cIf, uint8_t address,
              const void *buf, size_t count, int stop) {
  if (i2cIf >= N_I2C_IF) {
    return I2C_ERROR_ARG;
  }        
  I2C_TypeDef* pI2c = I2C_MAP[i2cIf].i2c;  

  /* Disable POS */
  pI2c->CR1 &= ~I2C_CR1_POS;
  
  /* Generate Start */
  pI2c->CR1 |= I2C_CR1_START;

  /* Wait until SB flag is set */
  if (!waitUntilBitSetSR1(pI2c, I2C_SR1_SB)){
  
    return I2C_ERROR_TIMEOUT;
  }

  /* Send slave address */
   pI2c->DR = address << 1;

  /* Wait until ADDR flag is set */
  if (!waitUntilBitSetSR1(pI2c, I2C_SR1_ADDR)) {
   
    return I2C_ERROR_TIMEOUT;
  }

  /* Clear ADDR flag */
  clearAddrFlag(pI2c);
  
  return i2c_write_data(i2cIf, buf, count, stop);
}
//-----------------------------------------------------------------------------
int i2c_write_data(HAL_I2C_Interface i2cIf, const void* buf, size_t count, int stop) {
  if (i2cIf >= N_I2C_IF) {
    return I2C_ERROR_ARG;
  }  
  I2C_TypeDef* pI2c = I2C_MAP[i2cIf].i2c;  
   
  const uint8_t* pData = buf;
  
  pI2c->SR1 = ~I2C_SR1_AF;
  
  int todo = count;
  while (todo > 0) {
    /* Wait until TXE flag is set */
    if (!waitUntilBitSetSR1(pI2c, I2C_SR1_TXE)) {
      return I2C_ERROR_TIMEOUT;
    }

    /* Write data to DR */
    pI2c->DR = *pData++;
    todo--;

    if ((pI2c->SR1 & I2C_SR1_BTF) && (todo != 0)) {
      /* Write data to DR */
      pI2c->DR = *pData++;
      todo--;
    }
  }

  /* Wait until TXE flag is set */
  if (!waitUntilBitSetSR1(pI2c, I2C_SR1_TXE)) {
    return I2C_ERROR_TIMEOUT;
  }
  
  /* May not need this test since TXE is not set if NACK is returned */
  if (pI2c->SR1 & I2C_SR1_AF) {
    return I2C_ERROR_ACK_FAILURE;
  }
  
  /* Generate Stop */
  if (stop) {
    pI2c->CR1 |= I2C_CR1_STOP;
    
    if (!waitForStopCondition(pI2c)) {
      return I2C_ERROR_TIMEOUT;
    }
  }

  return count;
}
