// crc32.h

#if !defined( CRC32_H )
#define       CRC32_H
//-----------------------------------------------------------------------------
//************************************
// Variables
//************************************

//************************************
// Prototypes
//************************************
void Do_CRC32(u32_t *crc32, u8_t input_data);
void Do_CRC32_Buf(u32_t *crc32, u16_t len, u8_t *buf);


//-----------------------------------------------------------------------------
#endif	//#if !defined( CRC8_H )
