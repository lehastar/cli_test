// crc16_8408.h

#if !defined( CRC16_8408_H )
#define       CRC16_8408_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern const u16_t crc16_8408_table [256];

#define ISO14443_CRC16_A_INIT 0x6363
#define ISO14443_CRC16_B_INIT 0xFFFF

//************************************
// Prototypes
//************************************
extern void Do_CRC16_8408(u16_t *crc16_8408, u8_t input_data);
extern void Do_CRC16_8408_Buf(u16_t *crc16_8408, u16_t len, u8_t *buf);
extern void Do_CRC16_ISO3309(u16_t *crc16_iso3309, u8_t input_data);
extern void Do_CRC16_ISO3309_Buf(u16_t *crc16_iso3309, u16_t len, u8_t *buf);

//-----------------------------------------------------------------------------
#endif	//#if !defined( CRC16_8408_H )
