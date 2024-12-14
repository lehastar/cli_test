//service.h

#if !defined( SERVICE_H )
#define       SERVICE_H
//-----------------------------------------------------------------------------

//************************************
// Variables
//************************************

//************************************
// Prototypes
//************************************
extern u8_t Get_ASCII(u8_t chr);
extern u8_t ASCII_To_Char(u8_t chr);
extern u8_t BCD_To_Char(u8_t bcd_data);
extern u16_t Char_To_BCD(u8_t char_data);
extern u32_t Int_To_BCD(u16_t int_data);
extern u8_t Parity(u8_t byte);
extern char InvertByte (char b);

extern int myatoi(char* s);
extern void uitoa(u16_t Value, u8_t* Buffer);

extern u16_t Rvr16(u16_t x);
extern u32_t Rvr32(u32_t x);
extern void Rvr64(u8_t *dst, u8_t *src);

//-----------------------------------------------------------------------------
#endif

