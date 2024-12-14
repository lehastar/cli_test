//eeprom.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "eeprom_defines.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************

//************************************
// Prototypes
//************************************
void EE_Unlock(void);
void EE_Erase(u32_t addr);
void EE_Write(u8_t* data, u32_t addr, u32_t len);
void EE_Read(u8_t* data, u32_t addr, u32_t len);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Разблокировка памяти
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void EE_Unlock(void)
{
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Стирание страницы
// addr - любой адрес, принадлежащий стираемой странице
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void EE_Erase(u32_t addr)
{
  while (FLASH->SR & FLASH_SR_BSY);
  if(FLASH->SR & FLASH_SR_EOP)
  {
    FLASH->SR = FLASH_SR_EOP;
  }

  FLASH->CR |= FLASH_CR_PER;
  FLASH->AR = addr;
  FLASH->CR |= FLASH_CR_STRT;
  while (!(FLASH->SR & FLASH_SR_EOP));
  FLASH->SR = FLASH_SR_EOP;
  FLASH->CR &= ~FLASH_CR_PER;
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Запись
// data - указатель на записываемые данные
// addr - адрес во flash
// count - количество записываемых байт, должно быть кратно 2
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void EE_Write(u8_t* data, u32_t addr, u32_t len)
{
  u32_t i;

  while (FLASH->SR & FLASH_SR_BSY);
  if (FLASH->SR & FLASH_SR_EOP)
  {
    FLASH->SR = FLASH_SR_EOP;
  }

  FLASH->CR |= FLASH_CR_PG;

  for (i = 0; i < len; i += 2)
  {
    *(volatile u16_t*)(addr + i) = (((u16_t)data[i + 1]) << 8) + data[i];
    while (!(FLASH->SR & FLASH_SR_EOP));
    FLASH->SR = FLASH_SR_EOP;
  }

  FLASH->CR &= ~(FLASH_CR_PG);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Чтение
// data - указатель на данные
// addr - адрес во flash
// count - количество записываемых байт, должно быть кратно 2
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void EE_Read(u8_t* data, u32_t addr, u32_t len)
{
  u32_t i;
  u16_t tmp;

  for (i = 0; i < len; i += 2)
  {
    tmp = *(volatile u16_t*)(addr + i);
    memcpy((u8_t*)&data[i], (u8_t*)&tmp, 2);
  }
}

//--------------------------------------------------------------------------------------------------------

