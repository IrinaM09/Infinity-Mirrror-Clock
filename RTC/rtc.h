
#ifndef _RTC_H_
#define _RTC_H_

typedef unsigned char   uint8_t;

#define C_Ds1307ReadMode_U8   0xD1u  // DS1307 ID
#define C_Ds1307WriteMode_U8  0xD0u  // DS1307 ID

#define C_Ds1307SecondRegAddress_U8   0x00u   // Address to access Ds1307 SEC register
#define C_Ds1307DateRegAddress_U8     0x04u   // Address to access Ds1307 DATE register
#define C_Ds1307ControlRegAddress_U8  0x07u   // Address to access Ds1307 CONTROL register

void RTC_Init();
void RTC_SetTime(uint8_t, uint8_t, uint8_t);
void RTC_GetTime(uint8_t *,uint8_t *,uint8_t *);

#endif
