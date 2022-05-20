#include "HDC2080.h"
#include "stddef.h"

static uint8_t _HDC2080_Temperature_Offset_to_reg(HDC2080_Temperature_Offset_t *temperature_offset);
static uint8_t _HDC2080_Humidity_Offset_to_reg(HDC2080_Humidity_Offset_t *humidity_offset); 

HDC2080_Status_t HDC2080_Init(HDC2080_t *HDC2080, uint8_t hdc2080_address, HDC2080_I2C_Read_t I2C_Read, HDC2080_I2C_Write_t I2C_Write)
{
	if ((HDC2080 == NULL) || (I2C_Read == NULL) || (I2C_Write == NULL) || ((hdc2080_address != HDC2080_ADDR_GND) && (hdc2080_address != HDC2080_ADDR_VDD)))
	{
		return HDC2080_ERROR;
	}

	HDC2080->address = hdc2080_address;
	HDC2080->I2C_Read = I2C_Read;
	HDC2080->I2C_Write = I2C_Write;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Start_Conversion(HDC2080_t *HDC2080)
{
	if (HDC2080 == NULL)
	{
		return HDC2080_ERROR;
	}
	
	return HDC2080_Set_Measurement_Trigger(HDC2080, HDC2080_Measurement_Trigger_Start);
}

HDC2080_Status_t HDC2080_Soft_Reset(HDC2080_t *HDC2080)
{
	if (HDC2080 == NULL)
	{
		return HDC2080_ERROR;
	}
	
	uint8_t reg = RESET_DRDY_INT_CONF_SOFT_RES;

	HDC2080->I2C_Write(HDC2080->address, RESET_DRDY_INT_CONF_REG, &reg); // set soft reset bit

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Temperature_Resolution(HDC2080_t *HDC2080, HDC2080_Temperature_Resolution_t resolution)
{
	if ((HDC2080 == NULL) || (resolution > 2) || (resolution < 0))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state 

	tmp_reg &= ~(MEASUREMENT_CONFIGURATION_TRES_0 | MEASUREMENT_CONFIGURATION_TRES_1); // clear TRES bits
	tmp_reg  |= resolution << 6; // set AMM rate bits

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Temperature_Resolution(HDC2080_t *HDC2080, HDC2080_Temperature_Resolution_t *resolution)
{
	if ((HDC2080 == NULL) || (resolution == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state 

	tmp_reg &= (MEASUREMENT_CONFIGURATION_TRES_0 | MEASUREMENT_CONFIGURATION_TRES_1); // clear all bits except resolution
	*resolution = tmp_reg >> 6;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Humidity_Resolution(HDC2080_t *HDC2080, HDC2080_Humidity_Resolution_t resolution)
{
	if ((HDC2080 == NULL) || (resolution > 2) || (resolution < 0))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(MEASUREMENT_CONFIGURATION_HRES_0 | MEASUREMENT_CONFIGURATION_HRES_1); // clear HRES bits
	tmp_reg  |= resolution << 4; // set AMM rate bits

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Humidity_Resolution(HDC2080_t *HDC2080, HDC2080_Humidity_Resolution_t *resolution)
{
	if ((HDC2080 == NULL) || (resolution == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= (MEASUREMENT_CONFIGURATION_HRES_0 | MEASUREMENT_CONFIGURATION_HRES_1); // clear all bits except resolution
	*resolution = tmp_reg >> 4;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_AMM_Rate(HDC2080_t *HDC2080, HDC2080_AMM_Rate_t rate)
{
	if ((HDC2080 == NULL) || (rate > 7) || (rate < 0))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(RESET_DRDY_INT_CONF_AMM_0 | RESET_DRDY_INT_CONF_AMM_1 | RESET_DRDY_INT_CONF_AMM_2); // clear AMM rate bits
	tmp_reg  |= rate << 4; // set AMM rate bits

	HDC2080->I2C_Write(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_AMM_Rate(HDC2080_t *HDC2080, HDC2080_AMM_Rate_t *rate)
{
	if ((HDC2080 == NULL) || (rate == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	tmp_reg &= (RESET_DRDY_INT_CONF_AMM_0 | RESET_DRDY_INT_CONF_AMM_1 | RESET_DRDY_INT_CONF_AMM_2); // // clear all bits except amm rate
	*rate = tmp_reg >> 4; // set AMM rate bits

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Temperature(HDC2080_t *HDC2080, float *temperature)
{
	if ((HDC2080 == NULL) || (temperature == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint16_t temp = 0;
	uint8_t temp_lh[2] = {0};
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080->I2C_Read(HDC2080->address, TEMPERATURE_LOW_REG, temp_lh, 2); // read TEMPERATURE_LOW and TEMPERATURE_HIGH registers

		temp = (uint16_t)temp_lh[1] << 8; // merge two 8bit variables to 16bit
		temp = temp | (uint16_t)temp_lh[0];

		*temperature = ((float)temp/65536)*165-40.5; // convert to degrees Celsius

		return HDC2080_OK;
	}
	else
	{
		return HDC2080_DNR;
	}
}

HDC2080_Status_t HDC2080_Get_Humidity(HDC2080_t *HDC2080, float *humidity)
{
	if ((HDC2080 == NULL) || (humidity == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint16_t hum = 0;
	uint8_t hum_lh[2] = {0};
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080->I2C_Read(HDC2080->address, HUMIDITY_LOW_REG, hum_lh, 2); // read HUMIDITY_LOW and HUMIDITY_HIGH registers

		hum = (uint16_t)hum_lh[1] << 8; // merge two 8bit variables to 16bit
		hum = hum | (uint16_t)hum_lh[0];

		*humidity = ((float)hum/65536)*100; // convert to %RH

		return HDC2080_OK;
	}
	else
	{
		return HDC2080_DNR;
	}
}

HDC2080_Status_t HDC2080_Get_Temperature_Humidity(HDC2080_t *HDC2080, float *temperature, float *humidity)
{
	if ((HDC2080 == NULL) || (temperature == NULL) || (humidity == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint16_t temp = 0;
	uint16_t hum = 0;
	uint8_t temp_hum[4] = {0};
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_DRDY_REG, &tmp_reg, 1); // read INTERRUPT_DRDY_REG register

	if((tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) == INTERRUPT_DRDY_DRDY_STATUS) // check if data is ready
	{
		HDC2080->I2C_Read(HDC2080->address, TEMPERATURE_LOW_REG, temp_hum, 4); // read TEMPERATURE_LOW and TEMPERATURE_HIGH registers

		temp = (uint16_t)temp_hum[1] << 8; // merge two 8bit variables to 16bit
		temp = temp | (uint16_t)temp_hum[0];

		hum = (uint16_t)temp_hum[3] << 8; // merge two 8bit variables to 16bit
		hum = hum | (uint16_t)temp_hum[2];

		*temperature = ((float)temp/65536)*165-40.5; // convert to degrees Celsius

		*humidity = ((float)hum/65536)*100; // convert to %RH

		return HDC2080_OK;
	}
	else
	{
		return HDC2080_DNR;
	}
}

HDC2080_Status_t HDC2080_Get_Max_Temperature(HDC2080_t *HDC2080, float *temperature)
{
	if ((HDC2080 == NULL) || (temperature == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t temp = 0;

	HDC2080->I2C_Read(HDC2080->address, TEMPERATURE_MAX_REG, &temp, 1); // read TEMPERATURE_MAX_REG register

	*temperature = ((float)temp/256)*165-40.5; // convert to degrees Celsius

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Max_Humidity(HDC2080_t *HDC2080, float *humidity)
{
	if ((HDC2080 == NULL) || (humidity == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t hum = 0;

	HDC2080->I2C_Read(HDC2080->address, HUMIDITY_MAX_REG, &hum, 1); // read HUMIDITY_MAX_REG register

	*humidity = ((float)hum/256)*100; // convert to %RH

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Temperature_Offset(HDC2080_t *HDC2080, HDC2080_Temperature_Offset_t *temperature_offset)
{
	if ((HDC2080 == NULL) || (temperature_offset == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t reg = _HDC2080_Temperature_Offset_to_reg(temperature_offset);
	
	HDC2080->I2C_Write(HDC2080->address, TEMP_OFFSET_ADJUST_REG, &reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Temperature_Offset(HDC2080_t *HDC2080, float *temperature_offset)
{
	if ((HDC2080 == NULL) || (temperature_offset == NULL))
	{
		return HDC2080_ERROR;
	}
	
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

HDC2080_Status_t HDC2080_Set_Humidity_Offset(HDC2080_t *HDC2080, HDC2080_Humidity_Offset_t *humidity_offset)
{
	if ((HDC2080 == NULL) || (humidity_offset == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t reg = _HDC2080_Humidity_Offset_to_reg(humidity_offset);

	HDC2080->I2C_Write(HDC2080->address, HUM_OFFSET_ADJUST_REG, &reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Humidity_Offset(HDC2080_t *HDC2080, float *humidity_offset)
{
	if ((HDC2080 == NULL) || (humidity_offset == NULL))
	{
		return HDC2080_ERROR;
	}
	
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

HDC2080_Status_t HDC2080_Set_Temperature_Threshold_Low(HDC2080_t *HDC2080, float temperature_threshold)
{
	if (HDC2080 == NULL)
	{
		return HDC2080_ERROR;
	}
	
	if ((temperature_threshold < -40.0) || (temperature_threshold > 125.0)) // check if parameter in range
	{
		return HDC2080_ERROR;
	}

	uint8_t reg = 0;
	float temp = temperature_threshold;

	temp += 40.5;
	temp /= 165;
	temp *= 256;

	reg = (uint8_t)temp;

	HDC2080->I2C_Write(HDC2080->address, TEMP_THR_L_REG, &reg);

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Temperature_Threshold_Low(HDC2080_t *HDC2080, float *temperature_threshold)
{
	if ((HDC2080 == NULL) || (temperature_threshold == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, TEMP_THR_L_REG, &tmp_reg, 1); // read register state

	*temperature_threshold = ((float)tmp_reg/256)*165-40.5;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Temperature_Threshold_High(HDC2080_t *HDC2080, float temperature_threshold)
{
	if (HDC2080 == NULL)
	{
		return HDC2080_ERROR;
	}
	
	if ((temperature_threshold < -40.0) || (temperature_threshold > 125.0)) // check if parameter in range
	{
		return HDC2080_ERROR;
	}
	
	uint8_t reg = 0;
	float temp = temperature_threshold;

	temp += 40.5;
	temp /= 165;
	temp *= 256;

	reg = (uint8_t)temp;

	HDC2080->I2C_Write(HDC2080->address, TEMP_THR_H_REG, &reg);

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Temperature_Threshold_High(HDC2080_t *HDC2080, float *temperature_threshold)
{
	if ((HDC2080 == NULL) || (temperature_threshold == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, TEMP_THR_H_REG, &tmp_reg, 1); // read register state

	*temperature_threshold = ((float)tmp_reg/256)*165-40.5;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Humidity_Threshold_Low(HDC2080_t *HDC2080, float humidity_threshold)
{
	if (HDC2080 == NULL)
	{
		return HDC2080_ERROR;
	}
	
	if((humidity_threshold < 0.0) || (humidity_threshold > 100.0)) // check if parameter in range
	{
		return HDC2080_ERROR;
	}

	uint8_t reg = 0;
	float temp = humidity_threshold;

	temp /= 100;
	temp *= 256;

	reg = (uint8_t)temp;

	HDC2080->I2C_Write(HDC2080->address, RH_THR_L_REG, &reg);

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Humidity_Threshold_Low(HDC2080_t *HDC2080, float *humidity_threshold)
{
	if ((HDC2080 == NULL) || (humidity_threshold == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RH_THR_L_REG, &tmp_reg, 1); // read register state

	*humidity_threshold = ((float)tmp_reg/256)*100;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Humidity_Threshold_High(HDC2080_t *HDC2080, float humidity_threshold)
{
	if (HDC2080 == NULL)
	{
		return HDC2080_ERROR;
	}
	
	if((humidity_threshold < 0.0) || (humidity_threshold > 100.0)) // check if parameter in range
	{
		return HDC2080_ERROR;
	}
	
	uint8_t reg = 0;
	float temp = humidity_threshold;

	temp /= 100;
	temp *= 256;

	reg = (uint8_t)temp;

	HDC2080->I2C_Write(HDC2080->address, RH_THR_H_REG, &reg);

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Humidity_Threshold_High(HDC2080_t *HDC2080, float *humidity_threshold)
{
	if ((HDC2080 == NULL) || (humidity_threshold == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RH_THR_H_REG, &tmp_reg, 1); // read register state

	*humidity_threshold = ((float)tmp_reg/256)*100;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Heater(HDC2080_t *HDC2080, HDC2080_Heater_t heater_state)
{
	if ((HDC2080 == NULL) || (heater_state > 1) || (heater_state < 0))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(RESET_DRDY_INT_CONF_HEAT_EN); // clear HEAT_EN bit
	tmp_reg  |= heater_state << 3; // set HEAT_EN bit

	HDC2080->I2C_Write(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Heater(HDC2080_t *HDC2080, HDC2080_Heater_t *heater_state)
{
	if ((HDC2080 == NULL) || (heater_state == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	tmp_reg &= RESET_DRDY_INT_CONF_HEAT_EN; // clear every bit except HEAT_EN bit
	*heater_state = tmp_reg >> 3; 
	
	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Measurement_Configuration(HDC2080_t *HDC2080, HDC2080_Measurement_Configuration_t configuration)
{
	if ((HDC2080 == NULL) || (configuration > 1) || (configuration < 0))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(MEASUREMENT_CONFIGURATION_MEAS_CONF_0 | MEASUREMENT_CONFIGURATION_MEAS_CONF_1); // clear MEAS_CONF bits
	tmp_reg  |= configuration << 2; // set MEAS_CONF bits

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Measurement_Configuration(HDC2080_t *HDC2080, HDC2080_Measurement_Configuration_t *configuration)
{
	if ((HDC2080 == NULL) || (configuration == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= MEASUREMENT_CONFIGURATION_MEAS_CONF_0 | MEASUREMENT_CONFIGURATION_MEAS_CONF_1; // clear every bit except MEAS_CONF bits
	*configuration = tmp_reg >> 2;

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Measurement_Trigger(HDC2080_t *HDC2080, HDC2080_Measurement_Trigger_t trigger)
{
	if ((HDC2080 == NULL) || (trigger > 1) || (trigger < 0))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~MEASUREMENT_CONFIGURATION_MEAS_TRIG; // clear MEAS_TRIG bit
	tmp_reg  |= trigger; // set MEAS_TRIG bit

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Measurement_Trigger(HDC2080_t *HDC2080, HDC2080_Measurement_Trigger_t *trigger)
{
	if ((HDC2080 == NULL) || (trigger == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg, 1); // read register state

	tmp_reg &= MEASUREMENT_CONFIGURATION_MEAS_TRIG; // clear every bit except MEAS_TRIG bit
	*trigger = tmp_reg;

	HDC2080->I2C_Write(HDC2080->address, MEASUREMENT_CONFIGURATION_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Manufacturer_ID(HDC2080_t *HDC2080, uint16_t *id)
{
	if ((HDC2080 == NULL) || (id == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t id_lh[2] = {0};
	uint16_t tmp_id = 0;

	HDC2080->I2C_Read(HDC2080->address, MANUFACTURER_ID_LOW_REG, id_lh, 2);

	tmp_id = (uint16_t)id_lh[1] << 8; // merge two 8bit variables to 16bit
	tmp_id = tmp_id | (uint16_t)id_lh[0];

	*id = tmp_id;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Device_ID(HDC2080_t *HDC2080, uint16_t *id)
{
	if ((HDC2080 == NULL) || (id == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t id_lh[2] = {0};
	uint16_t tmp_id = 0;
	
	HDC2080->I2C_Read(HDC2080->address, DEVICE_ID_LOW_REG, id_lh, 2);

	tmp_id = (uint16_t)id_lh[1] << 8; // merge two 8bit variables to 16bit
	tmp_id = tmp_id | (uint16_t)id_lh[0];

	*id = tmp_id;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_INT_Pin_Configuration(HDC2080_t *HDC2080, HDC2080_INT_Pin_Configuration_t *config)
{
	if ((HDC2080 == NULL) || (config == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	tmp_reg &= ~(RESET_DRDY_INT_CONF_DRDY_INT_EN | RESET_DRDY_INT_CONF_INT_POL | RESET_DRDY_INT_CONF_INT_MODE); // clear DRDY/INT_EN, INT_POL, INT_MODE bits
	tmp_reg  |= ((config->pin << 2) | (config->polarity << 1) | config->mode);

	HDC2080->I2C_Write(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_INT_Pin_Configuration(HDC2080_t *HDC2080, HDC2080_INT_Pin_Configuration_t *config)
{
	if ((HDC2080 == NULL) || (config == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, RESET_DRDY_INT_CONF_REG, &tmp_reg, 1); // read register state

	config->mode = tmp_reg & RESET_DRDY_INT_CONF_INT_MODE;
	config->polarity = (tmp_reg & RESET_DRDY_INT_CONF_INT_POL) >> 1;
	config->pin = (tmp_reg & RESET_DRDY_INT_CONF_DRDY_INT_EN)  >> 2;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Set_Interrupt_Configuration(HDC2080_t *HDC2080, HDC2080_Interrupts_t *config)
{
	if ((HDC2080 == NULL) || (config == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	tmp_reg = ((config->dataready << 7) | (config->temperature_thr_high << 6) | (config->temperature_thr_low << 5) |
				(config->humidity_thr_high << 4) | (config->humidity_thr_low << 3));

	HDC2080->I2C_Write(HDC2080->address, INTERRUPT_ENABLE_REG, &tmp_reg); // overwrite register

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Interrupt_Configuration(HDC2080_t *HDC2080, HDC2080_Interrupts_t *config)
{
	if ((HDC2080 == NULL) || (config == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_ENABLE_REG, &tmp_reg, 1); // read register state

	config->humidity_thr_low = (tmp_reg & INTERRUPT_ENABLE_HL_ENABLE) >> 3;
	config->humidity_thr_high = (tmp_reg & INTERRUPT_ENABLE_HH_ENABLE) >> 4;
	config->temperature_thr_low = (tmp_reg & INTERRUPT_ENABLE_TL_ENABLE) >> 5;
	config->temperature_thr_high = (tmp_reg & INTERRUPT_ENABLE_TL_ENABLE) >> 6;
	config->dataready = (tmp_reg & INTERRUPT_ENABLE_DRDY_ENABLE) >> 7;

	return HDC2080_OK;
}

HDC2080_Status_t HDC2080_Get_Active_Interrupt(HDC2080_t *HDC2080, HDC2080_Interrupts_t *active_interrupts)
{
	if ((HDC2080 == NULL) || (active_interrupts == NULL))
	{
		return HDC2080_ERROR;
	}
	
	uint8_t tmp_reg = 0;

	HDC2080->I2C_Read(HDC2080->address, INTERRUPT_DRDY_REG, &tmp_reg, 1); // read register state

	active_interrupts->humidity_thr_low = (tmp_reg & INTERRUPT_DRDY_HL_STATUS) >> 3;
	active_interrupts->humidity_thr_high = (tmp_reg & INTERRUPT_DRDY_HH_STATUS) >> 4;
	active_interrupts->temperature_thr_low = (tmp_reg & INTERRUPT_DRDY_TL_STATUS) >> 5;
	active_interrupts->temperature_thr_high = (tmp_reg & INTERRUPT_DRDY_TH_STATUS) >> 6;
	active_interrupts->dataready = (tmp_reg & INTERRUPT_DRDY_DRDY_STATUS) >> 7;

	return HDC2080_OK;
}

static uint8_t _HDC2080_Temperature_Offset_to_reg (HDC2080_Temperature_Offset_t *temperature_offset) 
{
	uint8_t reg = 0;
	
	if (temperature_offset->of_0_16 != 0)
	{
		reg = TEMP_OFFSET_ADJUST_0;
	}
	else if (temperature_offset->of_0_32 != 0)
	{
		reg |= TEMP_OFFSET_ADJUST_1;
	}
	else if (temperature_offset->of_0_64 != 0)
	{
		reg |= TEMP_OFFSET_ADJUST_2;
	}
	else if (temperature_offset->of_1_28 != 0)
	{
		reg |= TEMP_OFFSET_ADJUST_3;
	}
	else if (temperature_offset->of_2_58 != 0)
	{
		reg |= TEMP_OFFSET_ADJUST_4;
	}
	else if (temperature_offset->of_5_16 != 0)
	{
		reg |= TEMP_OFFSET_ADJUST_5;
	}
	else if (temperature_offset->of_10_32 != 0)
	{
		reg |= TEMP_OFFSET_ADJUST_6;
	}
	else if (temperature_offset->of_neg_20_32 != 0)
	{
		reg |= TEMP_OFFSET_ADJUST_7;
	}

	return reg;
}

static uint8_t _HDC2080_Humidity_Offset_to_reg (HDC2080_Humidity_Offset_t *humidity_offset)
{
	uint8_t reg = 0;
	
	if (humidity_offset->of_0_2 != 0)
	{
		reg = HUM_OFFSET_ADJUST_0;
	}
	else if (humidity_offset->of_0_4 != 0)
	{
		reg |= HUM_OFFSET_ADJUST_1;
	}
	else if (humidity_offset->of_0_8 != 0)
	{
		reg |= HUM_OFFSET_ADJUST_2;
	}
	else if (humidity_offset->of_1_6 != 0)
	{
		reg |= HUM_OFFSET_ADJUST_3;
	}
	else if (humidity_offset->of_3_1 != 0)
	{
		reg |= HUM_OFFSET_ADJUST_4;
	}
	else if (humidity_offset->of_6_3 != 0)
	{
		reg |= HUM_OFFSET_ADJUST_5;
	}
	else if (humidity_offset->of_12_5 != 0)
	{
		reg |= HUM_OFFSET_ADJUST_6;
	}
	else if (humidity_offset->of_neg_25_0 != 0)
	{
		reg |= HUM_OFFSET_ADJUST_7;
	}

	return reg;
}
