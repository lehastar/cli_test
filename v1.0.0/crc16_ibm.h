// crc16_ibm.h

#if !defined( CRC16_IBM_H )
#define       CRC16_IBM_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************
extern const u16_t crc16_ibm_table [256];

//************************************
// Prototypes
//************************************
extern void Do_CRC16_IBM(u16_t *crc16_ibm, u8_t input_data);
extern void Do_CRC16_IBM_Buf(u16_t *crc16_ibm, u16_t len, u8_t *buf);

//-----------------------------------------------------------------------------
#endif	//#if !defined( CRC16_IBM_H )
