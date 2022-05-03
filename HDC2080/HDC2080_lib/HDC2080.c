#include "HDC2080.h"

HDC2080_Status_t HDC2080_Init(HDC2080_t *HDC2080, uint8_t hdc2080_address, HDC2080_I2C_Read_t I2C_Read, HDC2080_I2C_Write_t I2C_Write)
{
	HDC2080->address = hdc2080_address;
	HDC2080->I2C_Read = I2C_Read;
	HDC2080->I2C_Write = I2C_Write;
	//HDC2080->INT_Callback = HDC2080_INT_Callback;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Temperature_Resolution(HDC2080_t *HDC2080, HDC2080_Temperature_Resolution_t resolution)
{
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state 

	tmp_reg &= ~(MEASUREMENT_CONFIGURATION_TRES_0|MEASUREMENT_CONFIGURATION_TRES_1); // clear TRES bits
	tmp_reg |= resolution<<6; // set AMM rate bits

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Humidity_Resolution(HDC2080_t *HDC2080, HDC2080_Humidity_Resolution_t resolution)
{
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(MEASUREMENT_CONFIGURATION_HRES_0|MEASUREMENT_CONFIGURATION_HRES_1);; // clear HRES bits
	tmp_reg |= resolution<<4; // set AMM rate bits

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_AMM_Rate(HDC2080_t *HDC2080, HDC2080_AMM_Rate_t rate)
{
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(RESET_DRDY_INT_CONF_AMM_0|RESET_DRDY_INT_CONF_AMM_1|RESET_DRDY_INT_CONF_AMM_2); // clear AMM rate bits
	tmp_reg |= rate<<4; // set AMM rate bits

	HDC2080->I2C_Write(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Start_Conversion(HDC2080_t *HDC2080)
{
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1);  // read register state

	tmp_reg |= MEASUREMENT_CONFIGURATION_MEAS_TRIG; // set measurement trigger bit

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Read_Temperature(HDC2080_t *HDC2080, float *temperature)
{
	uint16_t temp = 0;
	uint8_t temp_lh[2] = {0};
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080->I2C_Read(HDC2080->address, TEMPERATURE_LOW_REG, temp_lh, 2); // read TEMPERATURE_LOW and TEMPERATURE_HIGH registers

		temp = (uint16_t)temp_lh[1]<<8; // merge two 8bit variables to 16bit
		temp = temp|(uint16_t)temp_lh[0];

		*temperature = ((float)temp/65536)*165-40.5; // convert to degrees Celsius

		return HDC2080_OK;
	}
	else
	{
		return HDC2080_DNR;
	}
}

HDC2080_Status_t HDC2080_Read_Humidity(HDC2080_t *HDC2080, float *humidity)
{
	uint16_t hum = 0;
	uint8_t hum_lh[2] = {0};
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080->I2C_Read(HDC2080->address, HUMIDITY_LOW_REG, hum_lh, 2); // read HUMIDITY_LOW and HUMIDITY_HIGH registers

		hum = (uint16_t)hum_lh[1]<<8; // merge two 8bit variables to 16bit
		hum = hum|(uint16_t)hum_lh[0];

		*humidity = ((float)hum/65536)*100; // convert to %RH

		return HDC2080_OK;
	}
	else
	{
		return HDC2080_DNR;
	}
}

HDC2080_Status_t HDC2080_Read_Temperature_Humidity(HDC2080_t *HDC2080, float *temperature, float *humidity)
{
	uint16_t temp = 0;
	uint16_t hum = 0;
	uint8_t temp_hum[4] = {0};
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080->I2C_Read(HDC2080->address, TEMPERATURE_LOW_REG, temp_hum, 4); // read TEMPERATURE_LOW and TEMPERATURE_HIGH registers

		temp = (uint16_t)temp_hum[1]<<8; // merge two 8bit variables to 16bit
		temp = temp|(uint16_t)temp_hum[0];

		hum = (uint16_t)temp_hum[3]<<8; // merge two 8bit variables to 16bit
		hum = hum|(uint16_t)temp_hum[2];

		*temperature = ((float)temp/65536)*165-40.5; // convert to degrees Celsius

		*humidity = ((float)hum/65536)*100; // convert to %RH

		return HDC2080_OK;
	}
	else
	{
		return HDC2080_DNR;
	}
}

HDC2080_Status_t HDC2080_Read_Max_Temperature(HDC2080_t *HDC2080, float *temperature)
{
	uint8_t temp = 0;

	HDC2080->I2C_Read(HDC2080->address, TEMPERATURE_MAX_REG, &temp, 1); // read TEMPERATURE_MAX_REG register

	*temperature = ((float)temp/256)*165-40.5; // convert to degrees Celsius

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Read_Max_Humidity(HDC2080_t *HDC2080, float *humidity)
{
	uint8_t hum = 0;

	HDC2080->I2C_Read(HDC2080->address, HUMIDITY_MAX_REG, &hum, 1); // read HUMIDITY_MAX_REG register

	*humidity = ((float)hum/256)*100; // convert to %RH

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Temperature_Offset(HDC2080_t *HDC2080, float *temperature_offset)
{
	uint8_t offset_reg = 0;
	float offset = 0.0f;
	float temperature_offset_coeffs[8] = {0.16f, 0.32f, 0.64f, 1.28f, 2.58f, 5.16f, 10.32f, -20.32f};

	HDC2080->I2C_Read(HDC2080->address, TEMP_OFFSET_ADJUST_REG, &offset_reg, 1); // read TEMP_OFFSET_ADJUST_REG register

	for(uint8_t i = 0; i<8; i++)
	{
		if(((offset_reg >> i) & 0x01) == 0x01)
		{
			offset += temperature_offset_coeffs[i]; 
		}
	}

	*temperature_offset = offset;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Humidity_Offset(HDC2080_t *HDC2080, float *humidity_offset)
{
	uint8_t offset_reg = 0;
	float offset = 0.0f;
	float humidity_offset_coeffs[8] = {0.2f, 0.4f, 0.8f, 1.6f, 3.1f, 6.3f, 12.5f, -25.0f};

	HDC2080->I2C_Read(HDC2080->address, HUM_OFFSET_ADJUST_REG, &offset_reg, 1); // read HUM_OFFSET_ADJUST_REG register

	for(uint8_t i = 0; i<8; i++)
	{
		if(((offset_reg >> i) & 0x01) == 0x01)
		{
			offset += humidity_offset_coeffs[i]; 
		}
	}

	*humidity_offset = offset;

	return HDC2080_OK;
}

