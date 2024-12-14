// crc8.h

#if !defined( CRC8_H )
#define       CRC8_H
//-----------------------------------------------------------------------------
//************************************
// Variables
//************************************

//************************************
// Prototypes
//************************************
extern void Do_CRC8(u8_t *crc8, u8_t input_data);
extern void Do_CRC8_Buf(u8_t *crc8, u16_t len, u8_t *buf);
//extern void Do_EEPROM_CRC8_Buf(u8_t *crc8, u16_t len, u8_t __eeprom *buf);
//extern void Do_FLASH_CRC8_Buf(u8_t *crc8, u16_t len, u8_t __flash *buf);

//-----------------------------------------------------------------------------
#endif	//#if !defined( CRC8_H )

