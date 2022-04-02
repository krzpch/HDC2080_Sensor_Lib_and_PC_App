/*
 * HDC2080_Config.h
 *
 *  Created on: Mar 29, 2022
 *      Author: Michał Tomacha
 */

#ifndef _HDC2080_CONFIG_H_
#define _HDC2080_CONFIG_H_

//platform specific includes
#include "i2c.h"
//end platform specific includes

//#define ADDR_VDD /*uncomment this line if HDC2080 addr pin is connected to vdd*/
#ifndef ADDR_VDD
#define HDC2080_ADDR 0x80
#else
#define HDC2080_ADDR 0x82
#endif

#define HDC2080_I2C_Write(X, Y) HAL_I2C_Mem_Write(&hi2c1, HDC2080_ADDR, (X), 1, (uint8_t *)(Y), 1, 100)
#define HDC2080_I2C_Read(X, Y, Z) HAL_I2C_Mem_Read(&hi2c1, HDC2080_ADDR, (X), 1, (Y), (Z), 100)
#define HDC2080_Delay_ms(X) HAL_Delay(X)

//void HDC2080_I2C_Write(uint8_t reg_addr, uint8_t reg_val);
//void HDC2080_I2C_Read(uint8_t reg_addr, uint8_t* data, uint8_t data_len);

#endif /* _HDC2080_CONFIG_H_ */
