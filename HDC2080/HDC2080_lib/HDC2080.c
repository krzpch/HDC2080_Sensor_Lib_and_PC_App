#include "HDC2080.h"

HDC2080_Status HDC2080_Set_Temperature_Resolution(HDC2080_Temperature_Resolution resolution)
{
	uint8_t tmp_reg = 0;

	HDC2080_I2C_Read(MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(MEASUREMENT_CONFIGURATION_TRES_0|MEASUREMENT_CONFIGURATION_TRES_1); // clear TRES bits
	tmp_reg |= resolution<<6; // set AMM rate bits

	HDC2080_I2C_Write(MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status HDC2080_Set_Humidity_Resolution(HDC2080_Humidity_Resolution resolution)
{
	uint8_t tmp_reg = 0;

	HDC2080_I2C_Read(MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(MEASUREMENT_CONFIGURATION_HRES_0|MEASUREMENT_CONFIGURATION_HRES_1);; // clear HRES bits
	tmp_reg |= resolution<<4; // set AMM rate bits

	HDC2080_I2C_Write(RESET_DRDY_INT_CONF_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status HDC2080_Set_AMM_Rate(HDC2080_AMM_Rate rate)
{
	uint8_t tmp_reg = 0;

	HDC2080_I2C_Read(RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(RESET_DRDY_INT_CONF_AMM_0|RESET_DRDY_INT_CONF_AMM_1|RESET_DRDY_INT_CONF_AMM_2); // clear AMM rate bits
	tmp_reg |= rate<<4; // set AMM rate bits

	HDC2080_I2C_Write(RESET_DRDY_INT_CONF_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status HDC2080_Start_Conversion(void)
{
	uint8_t tmp_reg = 0;

	HDC2080_I2C_Read(MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1);  // read register state

	tmp_reg |= MEASUREMENT_CONFIGURATION_MEAS_TRIG; // set measurement trigger bit

	HDC2080_I2C_Write(MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status HDC2080_Read_Temperature(float* temperature)
{
	uint16_t temp = 0;
	uint8_t temp_lh[2] = {0};
	uint8_t tmp_reg = 0;

	HDC2080_I2C_Read(INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080_I2C_Read(TEMPERATURE_LOW_REG, temp_lh, 2); // read TEMPERATURE_LOW and TEMPERATURE_HIGH registers

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

HDC2080_Status HDC2080_Read_Humidity(float* humidity)
{
	uint16_t hum = 0;
	uint8_t hum_lh[2] = {0};
	uint8_t tmp_reg = 0;

	HDC2080_I2C_Read(INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080_I2C_Read(HUMIDITY_LOW_REG, hum_lh, 2); // read HUMIDITY_LOW and HUMIDITY_HIGH registers

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

HDC2080_Status HDC2080_Read_Temperature_Humidity(float* temperature, float* humidity)
{
	uint16_t temp = 0;
	uint16_t hum = 0;
	uint8_t temp_hum[4] = {0};
	uint8_t tmp_reg = 0;

	HDC2080_I2C_Read(INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080_I2C_Read(TEMPERATURE_LOW_REG, temp_hum, 4); // read TEMPERATURE_LOW and TEMPERATURE_HIGH registers

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

HDC2080_Status HDC2080_Read_Max_Temperature(float* temperature)
{
	uint8_t temp = 0;

	HDC2080_I2C_Read(TEMPERATURE_MAX_REG, temp, 1); // read TEMPERATURE_MAX_REG register

	*temperature = ((float)temp/256)*165-40.5; // convert to degrees Celsius

	return HDC2080_OK;
}

HDC2080_Status HDC2080_Read_Max_Humidity(float* humidity)
{
	uint8_t hum = 0;

	HDC2080_I2C_Read(HUMIDITY_MAX_REG, hum, 1); // read HUMIDITY_MAX_REG register

	*humidity = ((float)hum/256)*100; // convert to %RH

	return HDC2080_OK;
}

HDC2080_Status HDC2080_Get_Temperature_Offset(float* temperature_offset)
{
	uint8_t i_offset = 0;
	float f_offset = 0.0f;

	HDC2080_I2C_Read(TEMP_OFFSET_ADJUST_REG, i_offset, 1); // read TEMP_OFFSET_ADJUST_REG register

	for (uint8_t i = 0; i<8; i++)
	{
		i_offset = i_offset >> i;

		if((i_offset & 1) == 1)
		{
			if(i != 7)
			{
				if( i < 4)
				{
					f_offset += (0.16 * i);
				}
				else
				{
					f_offset += (1.28 * (i - 4));
				}
			}
			else
			{
				f_offset -= 20.62;
			}
		}
	}

	if( (offset & TEMP_OFFSET_ADJUST_0) == TEMP_OFFSET_ADJUST_0 )
	{
		f_offset += 0.16;
	}
	if( (offset & TEMP_OFFSET_ADJUST_1) == TEMP_OFFSET_ADJUST_1 )
	{
		f_offset += 0.32;
	}
	if( (offset & TEMP_OFFSET_ADJUST_2) == TEMP_OFFSET_ADJUST_2 )
	{
		f_offset += 0.64;
	}
	if( (offset & TEMP_OFFSET_ADJUST_3) == TEMP_OFFSET_ADJUST_3 )
	{
		f_offset += 1.28;
	}
	if( (offset & TEMP_OFFSET_ADJUST_4) == TEMP_OFFSET_ADJUST_4 )
	{
		f_offset += 2.58;
	}
	if( (offset & TEMP_OFFSET_ADJUST_5) == TEMP_OFFSET_ADJUST_5 )
	{
		f_offset += 5.16;
	}
	if( (offset & TEMP_OFFSET_ADJUST_6) == TEMP_OFFSET_ADJUST_6 )
	{
		f_offset += 10.32;
	}
	else if( (offset & TEMP_OFFSET_ADJUST_7) == TEMP_OFFSET_ADJUST_7 )
	{
		f_offset -= 20.62;
	}

	*temperature_offset = f_offset;

	return HDC2080_OK;
}

HDC2080_Status HDC2080_Get_Humidity_Offset(float* humidity_offset)
{
	uint8_t offset = 0;

	HDC2080_I2C_Read(HUM_OFFSET_ADJUST_REG, offset, 1); // read HUM_OFFSET_ADJUST_REG register

	return HDC2080_OK;
}

