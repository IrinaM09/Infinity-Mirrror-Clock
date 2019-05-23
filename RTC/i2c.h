
#ifndef _I2C_H
#define _I2C_H

#include <avr/io.h>
#include <util/delay.h>

typedef unsigned char   uint8_t;

void I2C_Init();
void I2C_Start();
void I2C_Stop(void);
void I2C_Write(uint8_t );
uint8_t I2C_Read(uint8_t);

#endif
