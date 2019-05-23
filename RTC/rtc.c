#include "rtc.h"
#include "i2c.h"


//This function is used to Initialize the Ds1307 RTC.
void RTC_Init()
{
	I2C_Init();                             // Initialize the I2c module.
	I2C_Start();                            // Start I2C communication

	I2C_Write(C_Ds1307WriteMode_U8);        // Connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(C_Ds1307ControlRegAddress_U8);// Select the Ds1307 ControlRegister to configure Ds1307

	I2C_Write(0x00);                        // Write 0x00 to Control register to disable SQW-Out

	I2C_Stop();                             // Stop I2C communication after initializing DS1307
}

//The I/P arguments should of BCD, like 0x12,0x39,0x26 for 12hr,39min and 26sec.			 
void RTC_SetTime(uint8_t var_hour_u8, uint8_t var_min_u8, uint8_t var_sec_u8)
{
	I2C_Start();                            // Start I2C communication

	I2C_Write(C_Ds1307WriteMode_U8);        // connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(C_Ds1307SecondRegAddress_U8); // Select the SEC RAM address

	I2C_Write(var_sec_u8);			        // Write sec from RAM address 00H
	I2C_Write(var_min_u8);			        // Write min from RAM address 01H
	I2C_Write(var_hour_u8);			        // Write hour from RAM address 02H

	I2C_Stop();           	                // Stop I2C communication after Setting the Time
}


//The time read from Ds1307 will be of BCD format, like 0x12,0x39,0x26 for 12hr,39min and 26sec.	
void RTC_GetTime(uint8_t *ptr_hour_u8,uint8_t *ptr_min_u8,uint8_t *ptr_sec_u8)
{
	I2C_Start();                            // Start I2C communication

	I2C_Write(C_Ds1307WriteMode_U8);	    // connect to DS1307 by sending its ID on I2c Bus
	I2C_Write(C_Ds1307SecondRegAddress_U8); // Request Sec RAM address at 00H

	I2C_Stop();			                    // Stop I2C communication after selecting Sec Register

	I2C_Start();		                    // Start I2C communication
	I2C_Write(C_Ds1307ReadMode_U8);	        // connect to DS1307(Read mode) by sending its ID

	*ptr_sec_u8 = I2C_Read(1);                // read second and return Positive ACK
	*ptr_min_u8 = I2C_Read(1); 	            // read minute and return Positive ACK
	*ptr_hour_u8 = I2C_Read(0);               // read hour and return Negative/No ACK

	I2C_Stop();		                        // Stop I2C communication after reading the Time
}
