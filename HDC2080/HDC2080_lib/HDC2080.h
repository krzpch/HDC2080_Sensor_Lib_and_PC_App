/*
 * HDC2080.h
 *
 *  Created on: Mar 22, 2022
 *      Author: Michał Tomacha
 */

#ifndef _HDC2080_H_
#define _HDC2080_H_

#include "stdint.h"

#define TEMPERATURE_LOW_REG                 0x00
#define TEMPERATURE_HIGH_REG                0x01
#define HUMIDITY_LOW_REG                    0x02
#define HUMIDITY_HIGH_REG                   0x03
#define INTERRUPT_DRDY_REG                  0x04
#define TEMPERATURE_MAX_REG                 0x05
#define HUMIDITY_MAX_REG                    0x06
#define INTERRUPT_ENABLE_REG                0x07
#define TEMP_OFFSET_ADJUST_REG              0x08
#define HUM_OFFSET_ADJUST_REG               0x09
#define TEMP_THR_L_REG                      0x0A
#define TEMP_THR_H_REG                      0x0B
#define RH_THR_L_REG                        0x0C
#define RH_THR_H_REG                        0x0D
#define RESET_DRDY_INT_CONF_REG             0x0E
#define MEASUREMENT_CONFIGURATION_REG       0x0F
#define MANUFACTURER_ID_LOW_REG             0xFC
#define MANUFACTURER_ID_HIGH_REG            0xFD
#define DEVICE_ID_LOW_REG                   0xFE
#define DEVICE_ID_HIGH_REG                  0xFF

#define INTERRUPT_DRDY_DRDY_STATUS          0x80
#define INTERRUPT_DRDY_TH_STATUS            0x40
#define INTERRUPT_DRDY_TL_STATUS            0x20  
#define INTERRUPT_DRDY_HH_STATUS            0x10   
#define INTERRUPT_DRDY_HL_STATUS            0x08

#define INTERRUPT_ENABLE_DRDY_ENABLE        0x80
#define INTERRUPT_ENABLE_TH_ENABLE          0x40
#define INTERRUPT_ENABLE_TL_ENABLE          0x20
#define INTERRUPT_ENABLE_HH_ENABLE          0x10
#define INTERRUPT_ENABLE_HL_ENABLE          0x08

#define TEMP_OFFSET_ADJUST_7        0x80
#define TEMP_OFFSET_ADJUST_6        0x40
#define TEMP_OFFSET_ADJUST_5        0x20 
#define TEMP_OFFSET_ADJUST_4        0x10
#define TEMP_OFFSET_ADJUST_3        0x08
#define TEMP_OFFSET_ADJUST_2        0x04
#define TEMP_OFFSET_ADJUST_1        0x02
#define TEMP_OFFSET_ADJUST_0        0x01
 
#define HUM_OFFSET_ADJUST_7         0x80       
#define HUM_OFFSET_ADJUST_6         0x40  
#define HUM_OFFSET_ADJUST_5         0x20  
#define HUM_OFFSET_ADJUST_4         0x10  
#define HUM_OFFSET_ADJUST_3         0x08  
#define HUM_OFFSET_ADJUST_2         0x04
#define HUM_OFFSET_ADJUST_1         0x02
#define HUM_OFFSET_ADJUST_0         0x01  

#define RESET_DRDY_INT_CONF_SOFT_RES        0x80
#define RESET_DRDY_INT_CONF_AMM_2           0x40
#define RESET_DRDY_INT_CONF_AMM_1           0x20
#define RESET_DRDY_INT_CONF_AMM_0           0x10
#define RESET_DRDY_INT_CONF_HEAT_EN         0x08
#define RESET_DRDY_INT_CONF_DRDY_INT_EN     0x04
#define RESET_DRDY_INT_CONF_INT_POL         0x02
#define RESET_DRDY_INT_CONF_INT_MODE        0x01

#define MEASUREMENT_CONFIGURATION_TRES_1         0x80
#define MEASUREMENT_CONFIGURATION_TRES_0         0x40
#define MEASUREMENT_CONFIGURATION_HRES_1         0x20
#define MEASUREMENT_CONFIGURATION_HRES_0         0x10
#define MEASUREMENT_CONFIGURATION_MEAS_CONF_1    0x04
#define MEASUREMENT_CONFIGURATION_MEAS_CONF_0    0x02
#define MEASUREMENT_CONFIGURATION_MEAS_TRIG      0x01

#define HDC2080_ADDR_GND 	0x80 // device i2c address when ADDR pin is unconnected or hardwired to GND
#define HDC2080_ADDR_VDD 	0x82 // device i2c address when ADDR pin is hardwired to VDD

typedef enum HDC2080_Status_t {
	HDC2080_OK = 0,
	HDC2080_ERROR = 1,
	HDC2080_DNR = 2 // Data Not Ready
} HDC2080_Status_t;

typedef HDC2080_Status_t (*HDC2080_I2C_Read_t)(uint16_t hdc2080_address, uint16_t hdc2080_reg, uint8_t *data, uint16_t data_size);
typedef HDC2080_Status_t (*HDC2080_I2C_Write_t)(uint16_t hdc2080_address, uint16_t hdc2080_reg, uint8_t *data);

typedef struct HDC2080_t {
	uint8_t address;
	HDC2080_I2C_Read_t I2C_Read;
	HDC2080_I2C_Write_t I2C_Write;
} HDC2080_t;

typedef enum HDC2080_Resolution_t {
	HDC2080_Temperature_Resolution_14_bit	 = 0,
	HDC2080_Temperature_Resolution_11_bit	 = 1,
	HDC2080_Temperature_Resolution_9_bit	 = 2
} HDC2080_Resolution_t;

typedef enum HDC2080_AMM_Rate_t { 	  // Auto measurement mode
	HDC2080_AMM_Rate_1_Disabled	 = 0, // disabled (measurement must be initiated via I2C)
	HDC2080_AMM_Rate_1_120Hz	 = 1, // 1/120 Hz
	HDC2080_AMM_Rate_1_60Hz		 = 2, // 1/60 Hz
	HDC2080_AMM_Rate_1_10Hz		 = 3, // 1/10 Hz
	HDC2080_AMM_Rate_1_5Hz		 = 4, // 1/5 Hz
	HDC2080_AMM_Rate_1Hz		 = 5, // 1 Hz
	HDC2080_AMM_Rate_2Hz		 = 6, // 2 Hz
	HDC2080_AMM_Rate_5Hz		 = 7  // 5 Hz
} HDC2080_AMM_Rate_t;

typedef struct HDC2080_Temperature_Offset_t {
	uint8_t of_0_16;		// offset 0.16
	uint8_t of_0_32;		// offset 0.32
	uint8_t of_0_64;		// offset 0.64
	uint8_t of_1_28;		// offset 1.28
	uint8_t of_2_58;		// offset 2.58
	uint8_t of_5_16;		// offset 5.16
	uint8_t of_10_32;		// offset 10.32
	uint8_t of_neg_20_32;	// offset -20.32
} HDC2080_Temperature_Offset_t;

typedef struct HDC2080_Humidity_Offset_t {
	uint8_t of_0_2;			// offset 0.2
	uint8_t of_0_4;			// offset 0.4
	uint8_t of_0_8;			// offset 0.8
	uint8_t of_1_6;			// offset 1.6
	uint8_t of_3_1;			// offset 3.1
	uint8_t of_6_3;			// offset 6.3
	uint8_t of_12_5;		// offset 12.5
	uint8_t of_neg_25_0;	// offset -25.0
} HDC2080_Humidity_Offset_t;

typedef enum HDC2080_Heater_t {
	HDC2080_Heater_Off	 = 0,
	HDC2080_Heater_On	 = 1
} HDC2080_Heater_t;

typedef enum HDC2080_Measurement_Configuration_t {
	HDC2080_Measurement_Configuration_Humidity_Temperature	= 0,
	HDC2080_Measurement_Configuration_Temperature	 		= 1
} HDC2080_Measurement_Configuration_t;

typedef enum HDC2080_Measurement_Trigger_t {
	HDC2080_Measurement_Trigger_No_Action	= 0,
	HDC2080_Measurement_Trigger_Start	 	= 1
} HDC2080_Measurement_Trigger_t;

typedef enum HDC2080_Pin_Configuration_t {
	HDC2080_Pin_Configuration_High_Z	= 0,
	HDC2080_Pin_Configuration_Enable	= 1
} HDC2080_Pin_Configuration_t;

typedef enum HDC2080_Pin_Interrupt_Polarity_t {
	HDC2080_Pin_Configuration_Active_Low	= 0,
	HDC2080_Pin_Configuration_Active_High	= 1
} HDC2080_Pin_Interrupt_Polarity_t;

typedef enum HDC2080_Pin_Interrupt_Mode_t {
	HDC2080_Pin_Configuration_Level_Sensitive	= 0,
	HDC2080_Pin_Configuration_Comparator_Mode	= 1
} HDC2080_Pin_Interrupt_Mode_t;

typedef struct HDC2080_INT_Pin_Configuration_t {
	HDC2080_Pin_Configuration_t pin;
	HDC2080_Pin_Interrupt_Polarity_t polarity;
	HDC2080_Pin_Interrupt_Mode_t mode;
} HDC2080_INT_Pin_Configuration_t;

typedef enum HDC2080_Interrupt_State_t {
	HDC2080_Interrupt_State_Disabled	= 0,
	HDC2080_Interrupt_State_Enabled		= 1
} HDC2080_Interrupt_State_t;

typedef struct HDC2080_Interrupts_t {
	HDC2080_Interrupt_State_t dataready;
	HDC2080_Interrupt_State_t temperature_thr_high;
	HDC2080_Interrupt_State_t temperature_thr_low;
	HDC2080_Interrupt_State_t humidity_thr_high;
	HDC2080_Interrupt_State_t humidity_thr_low;
} HDC2080_Interrupts_t;

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param hdc2080_address 
 * @param I2C_Read 
 * @param I2C_Write 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Init(HDC2080_t *HDC2080, uint8_t hdc2080_address, HDC2080_I2C_Read_t I2C_Read, HDC2080_I2C_Write_t I2C_Write);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Start_Conversion(HDC2080_t *HDC2080);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Soft_Reset(HDC2080_t *HDC2080);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param resolution 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Temperature_Resolution(HDC2080_t *HDC2080, HDC2080_Resolution_t resolution);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param resolution 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Temperature_Resolution(HDC2080_t *HDC2080, HDC2080_Resolution_t *resolution);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param resolution 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Humidity_Resolution(HDC2080_t *HDC2080, HDC2080_Resolution_t resolution);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param resolution 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Humidity_Resolution(HDC2080_t *HDC2080, HDC2080_Resolution_t *resolution);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param rate 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_AMM_Rate(HDC2080_t *HDC2080, HDC2080_AMM_Rate_t rate);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param rate 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_AMM_Rate(HDC2080_t *HDC2080, HDC2080_AMM_Rate_t *rate);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Temperature(HDC2080_t *HDC2080, float *temperature);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Humidity(HDC2080_t *HDC2080, float *humidity);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature 
 * @param humidity 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Temperature_Humidity(HDC2080_t *HDC2080, float *temperature, float *humidity);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Max_Temperature(HDC2080_t *HDC2080, float *temperature);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Max_Humidity(HDC2080_t *HDC2080, float *humidity);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature_offset 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Temperature_Offset(HDC2080_t *HDC2080, HDC2080_Temperature_Offset_t *temperature_offset);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature_offset 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Temperature_Offset(HDC2080_t *HDC2080, float *temperature_offset);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity_offset 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Humidity_Offset(HDC2080_t *HDC2080, HDC2080_Humidity_Offset_t *humidity_offset);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity_offset 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Humidity_Offset(HDC2080_t *HDC2080, float *humidity_offset);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Temperature_Threshold_Low(HDC2080_t *HDC2080, float temperature_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Temperature_Threshold_Low(HDC2080_t *HDC2080, float *temperature_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Temperature_Threshold_High(HDC2080_t *HDC2080, float temperature_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param temperature_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Temperature_Threshold_High(HDC2080_t *HDC2080, float *temperature_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Humidity_Threshold_Low(HDC2080_t *HDC2080, float humidity_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Humidity_Threshold_Low(HDC2080_t *HDC2080, float *humidity_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Humidity_Threshold_High(HDC2080_t *HDC2080, float humidity_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param humidity_threshold 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Humidity_Threshold_High(HDC2080_t *HDC2080, float *humidity_threshold);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param heater_state 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Heater(HDC2080_t *HDC2080, HDC2080_Heater_t heater_state);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param heater_state 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Heater(HDC2080_t *HDC2080, HDC2080_Heater_t *heater_state);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param configuration 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Measurement_Configuration(HDC2080_t *HDC2080, HDC2080_Measurement_Configuration_t configuration);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param configuration 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Measurement_Configuration(HDC2080_t *HDC2080, HDC2080_Measurement_Configuration_t *configuration);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param trigger 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Measurement_Trigger(HDC2080_t *HDC2080, HDC2080_Measurement_Trigger_t trigger);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param trigger 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Measurement_Trigger(HDC2080_t *HDC2080, HDC2080_Measurement_Trigger_t *trigger);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param id 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Manufacturer_ID(HDC2080_t *HDC2080, uint16_t *id);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param id 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Device_ID(HDC2080_t *HDC2080, uint16_t *id);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param config 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_INT_Pin_Configuration(HDC2080_t *HDC2080, HDC2080_INT_Pin_Configuration_t *config);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param config 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_INT_Pin_Configuration(HDC2080_t *HDC2080, HDC2080_INT_Pin_Configuration_t *config);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param config 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Set_Interrupt_Configuration(HDC2080_t *HDC2080, HDC2080_Interrupts_t *config);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param config 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Interrupt_Configuration(HDC2080_t *HDC2080, HDC2080_Interrupts_t *config);

/**
 * @brief 
 * 
 * @param HDC2080 
 * @param interrupts 
 * @return HDC2080_Status_t 
 */
HDC2080_Status_t HDC2080_Get_Active_Interrupt(HDC2080_t *HDC2080, HDC2080_Interrupts_t *interrupts);

#endif /* _HDC2080_H_ */
