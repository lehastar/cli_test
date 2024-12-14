//spi.c

#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include <string.h>

#include "settings.h"
#include "pins.h"
#include "service.h"

#include "spi_defines.h"

//--------------------------------------------------------------------------------------------------------

//************************************
// Variables
//************************************

//************************************
// Prototypes
//************************************
void SPI_Open(void);
u8_t SPI_IO(u8_t b);

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Инициализация
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void SPI_Open(void)
{
  SPI_InitTypeDef SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  //SCK -  PA5
  GPIO_InitStructure.GPIO_Pin = SPI1_SCK_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI1_SCK_GPIO_PORT, &GPIO_InitStructure);

  //MOSI - PA7
  GPIO_InitStructure.GPIO_Pin = SPI1_MOSI_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI1_MOSI_GPIO_PORT, &GPIO_InitStructure);

  //MISO - PA6
  GPIO_InitStructure.GPIO_Pin = SPI1_MISO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SPI1_MISO_GPIO_PORT, &GPIO_InitStructure);

  //SPI
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_Cmd(SPI1, ENABLE);
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Записываем байт
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
u8_t SPI_IO(u8_t b)
{
  while (!(SPI1->SR & SPI_SR_TXE));
  SPI1->DR = b;
  while (!(SPI1->SR & SPI_SR_RXNE));
  return (SPI1->DR);
}

//--------------------------------------------------------------------------------------------------------

