/**********************************************************************************************
* Includes
***********************************************************************************************/
#include "cli_hdc2080.h"
#include "HDC2080.h"
#include "usart.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**********************************************************************************************
* Module Preprocessor defines
***********************************************************************************************/
#if (TARGET == HDC2080_F407)
#define UART_HANDLER    (&huart4)
#define I2C_HANDLER     (&hi2c1)
#else
#define UART_HANDLER    (&huart2)
#define I2C_HANDLER     (&hi2c1)
#endif

#define HDC2080_CLI_BUFFER_SIZE (512U)

/**********************************************************************************************
* Module Preprocessor Macros
***********************************************************************************************/
#ifdef UNIT_TESTS
#define my_printf(X, Y...)                                     \
    do                                                         \
    {                                                          \
        int len = snprintf(NULL, 0, X, ##Y);                   \
        char *data = (char *)malloc((len + 1) * sizeof(char)); \
        sprintf(data, X, ##Y);                                 \
        CLI_Print(&HDC2080_CLI, data);                         \
        free(data);                                            \
    } while (0)
#else
#define my_printf(X, Y...) printf(X, ##Y)
#endif

/**********************************************************************************************
* Module Typedefs
***********************************************************************************************/

/**********************************************************************************************
* Function Prototypes
***********************************************************************************************/
static void HDC_hex_to_offset_temp(uint8_t hex, HDC2080_Temperature_Offset_t *offset);
static void HDC_hex_to_offset_hum(uint8_t hex, HDC2080_Humidity_Offset_t *offset);

static void HDC_CLI_Print(char *c);
static void HDC_CLI_Cmd_Get_Temperature(int argc, char **argv);
static void HDC_CLI_Cmd_Get_Humidity(int argc, char **argv);
static void HDC_CLI_Cmd_Get_Measurements(int argc, char **argv);
static void HDC_CLI_Cmd_Config_Temperature(int argc, char **argv);
static void HDC_CLI_Cmd_Config_Humidity(int argc, char **argv);
static void HDC_CLI_Cmd_Config_Interrupts(int argc, char **argv);
static void HDC_CLI_Cmd_Init(int argc, char **argv);
static void HDC_CLI_Cmd_Deinit(int argc, char **argv);
static void HDC_CLI_Cmd_Force_MEasurement(int argc, char **argv);
static HDC2080_Status_t HDC2080_I2C_Read(uint16_t hdc2080_address, uint16_t hdc2080_reg, uint8_t *data, uint16_t data_size);
static HDC2080_Status_t HDC2080_I2C_Write(uint16_t hdc2080_address, uint16_t hdc2080_reg, uint8_t *data);

/**********************************************************************************************
* Module Variable Definitions
***********************************************************************************************/
static CLI_Cmd_t HDC2080_Cmds[] =
{
    {.command = "get_temperature",      .cmd_handler = &HDC_CLI_Cmd_Get_Temperature},
    {.command = "get_humidity",         .cmd_handler = &HDC_CLI_Cmd_Get_Humidity},
    {.command = "get_measurements",     .cmd_handler = &HDC_CLI_Cmd_Get_Measurements},
    {.command = "config_temperature",   .cmd_handler = &HDC_CLI_Cmd_Config_Temperature},
    {.command = "config_humidity",      .cmd_handler = &HDC_CLI_Cmd_Config_Humidity},
    {.command = "config_interrupts",    .cmd_handler = &HDC_CLI_Cmd_Config_Interrupts},
    {.command = "sensor_initialize",    .cmd_handler = &HDC_CLI_Cmd_Init},
    {.command = "sensor_deinitialize",  .cmd_handler = &HDC_CLI_Cmd_Deinit},
    {.command = "force_measurement",    .cmd_handler = &HDC_CLI_Cmd_Force_MEasurement}
};

static char HDC2080_CLI_Rx_Buffer[HDC2080_CLI_BUFFER_SIZE] = {0};

CLI_t HDC2080_CLI =
{
    .queue = {.buffer = HDC2080_CLI_Rx_Buffer, .size = HDC2080_CLI_BUFFER_SIZE},
    .func_print = &HDC_CLI_Print,
    .commands = HDC2080_Cmds,
    .commands_nb = sizeof(HDC2080_Cmds) / sizeof(HDC2080_Cmds[0])
};

static HDC2080_t hdc =
{
    .address = HDC2080_ADDR_GND,
    .I2C_Read = &HDC2080_I2C_Read,
    .I2C_Write = &HDC2080_I2C_Write
};

/**********************************************************************************************
* Function Definitions
***********************************************************************************************/
static HDC2080_Status_t HDC2080_I2C_Read(uint16_t hdc2080_address, uint16_t hdc2080_reg, uint8_t *data, uint16_t data_size)
{
    return (HDC2080_Status_t)HAL_I2C_Mem_Read(I2C_HANDLER, hdc2080_address, hdc2080_reg, 1U , data, data_size, 1000);
}

static HDC2080_Status_t HDC2080_I2C_Write(uint16_t hdc2080_address, uint16_t hdc2080_reg, uint8_t *data)
{
    return (HDC2080_Status_t)HAL_I2C_Mem_Write(I2C_HANDLER, hdc2080_address, hdc2080_reg, 1U, data, 1U, 1000);
}

static void HDC_CLI_Print(char *c)
{
    HAL_UART_Transmit(UART_HANDLER, (uint8_t*)c, 1U, 10U);
}

static void HDC_CLI_Cmd_Get_Temperature(int argc, char **argv)
{
	float temp;
	HDC2080_Status_t ret;
	ret = HDC2080_Get_Temperature(&hdc, &temp);
	if(ret == HDC2080_OK)
	{
		my_printf("TEMP %.1f\n", temp);
	}
	else
	{
		my_printf("Bad Temp Meas, ECODE: %d\n", ret);
	}
}

static void HDC_CLI_Cmd_Get_Humidity(int argc, char **argv)
{
	float hum;
	HDC2080_Status_t ret;
	ret = HDC2080_Get_Humidity(&hdc, &hum);
	if (ret == HDC2080_OK) {
		my_printf("HUM %.1f\n", hum);
	} else {
		my_printf("Bad Hum Meas, ECODE: %d\n", ret);
	}
}

static void HDC_CLI_Cmd_Get_Measurements(int argc, char **argv)
{
	HDC2080_Status_t ret;
	float temp;
	float hum;

	ret = HDC2080_Get_Temperature_Humidity(&hdc, &temp, &hum);
	if (ret == HDC2080_OK) {
		my_printf("TEMP %.1f\nHUM %.1f\n", temp, hum);
	} else {
		my_printf("HDC2080_Get_Temperature_Humidity, ECODE: %d\n", ret);
	}
}

static void HDC_CLI_Cmd_Config_Temperature(int argc, char **argv)
{
	HDC2080_Status_t ret;
	uint8_t offset = 0x00;
	HDC2080_Temperature_Offset_t temp_offset = {0};

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "res9bit"))
		{
			ret = HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Resolution_9_bit);
			if (ret == HDC2080_OK) {
				my_printf("res9bit OK\n");
			} else {
				my_printf("res9bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res11bit"))
		{
			ret = HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Resolution_11_bit);
			if (ret == HDC2080_OK) {
				my_printf("res11bit OK\n");
			} else {
				my_printf("res11bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res14bit"))
		{
			ret = HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Resolution_14_bit);
			if (ret == HDC2080_OK) {
				my_printf("res14bit OK\n");
			} else {
				my_printf("res14bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "temp_offset"))
		{
			offset = strtol(argv[++i], NULL, 16);
			HDC_hex_to_offset_temp(offset, &temp_offset);
			ret = HDC2080_Set_Temperature_Offset(&hdc, &temp_offset);

			if (ret == HDC2080_OK)
			{
				my_printf("temp_offset OK, offset: %x\n",offset);
			}
			else
			{
				my_printf("temp_offset ECODE: %d, offset: %x\n", ret, offset);
			}
		}
		else
		{
			my_printf("Bad argument: %s\n",argv[i]);
		}
	}
}

static void HDC_CLI_Cmd_Config_Humidity(int argc, char **argv)
{
	HDC2080_Status_t ret;
	uint8_t offset = 0x00;
	HDC2080_Humidity_Offset_t temp_offset = {0};

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "res9bit"))
		{
			ret = HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Resolution_9_bit);
			if (ret == HDC2080_OK) {
				my_printf("res9bit OK\n");
			} else {
				my_printf("res9bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res11bit"))
		{
			ret = HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Resolution_11_bit);
			if (ret == HDC2080_OK) {
				my_printf("res11bit OK\n");
			} else {
				my_printf("res11bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res14bit"))
		{
			ret = HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Resolution_14_bit);
			if (ret == HDC2080_OK) {
				my_printf("res14bit OK\n");
			} else {
				my_printf("res14bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "hum_offset"))
		{
			offset = strtol(argv[++i], NULL, 16);
			HDC_hex_to_offset_hum(offset, &temp_offset);
			ret = HDC2080_Set_Humidity_Offset(&hdc, &temp_offset);

			if (ret == HDC2080_OK)
			{
				my_printf("hum_offset OK, offset: %x\n",offset);
			}
			else
			{
				my_printf("hum_offset ECODE: %d, offset: %x\n", ret, offset);
			}
		}
		else
		{
			my_printf("Bad argument: %s\n",argv[i]);
		}
	}
}

static void HDC_CLI_Cmd_Init(int argc, char **argv)
{
	HDC2080_Status_t ret;
	ret = HDC2080_Soft_Reset(&hdc);
	if (ret == HDC2080_OK) {
		my_printf("softRes OK\n");
	} else {
		my_printf("softRes ECODE: %d\n", ret);
	}

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "temp&hum"))
		{
	        ret = HDC2080_Set_Measurement_Configuration(&hdc, HDC2080_Measurement_Configuration_Humidity_Temperature);
			if (ret == HDC2080_OK) {
				my_printf("temp&hum OK\n");
			} else {
				my_printf("temp&hum ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "onlyTemp"))
		{
	        ret = HDC2080_Set_Measurement_Configuration(&hdc, HDC2080_Measurement_Configuration_Temperature);
			if (ret == HDC2080_OK) {
				my_printf("onlyTemp OK\n");
			} else {
				my_printf("onlyTemp ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "autoOff"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1_Disabled);
			if (ret == HDC2080_OK) {
				my_printf("autoOff OK\n");
			} else {
				my_printf("autoOff ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "1/120Hz"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1_120Hz);
			if (ret == HDC2080_OK) {
				my_printf("120Hz OK\n");
			} else {
				my_printf("120Hz ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "1/10Hz"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1_10Hz);
			if (ret == HDC2080_OK) {
				my_printf("10Hz OK\n");
			} else {
				my_printf("10Hz ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "1Hz"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1Hz);
			if (ret == HDC2080_OK) {
				my_printf("1Hz OK\n");
			} else {
				my_printf("1Hz ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "5Hz"))
		{
			ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_5Hz);
			if (ret == HDC2080_OK) {
				my_printf("5Hz OK\n");
			} else {
				my_printf("5Hz ECODE: %d\n", ret);
			}
		}
		else
		{
			my_printf("Bad argument: %s\n",argv[i]);
		}
	}
}

static void HDC_CLI_Cmd_Config_Interrupts(int argc, char **argv)
{
	HDC2080_Status_t ret;
	HDC2080_INT_Pin_Configuration_t interrupt_pin = {
		.pin = HDC2080_Pin_Configuration_High_Z,
		.polarity = HDC2080_Pin_Configuration_Active_Low,
		.mode = HDC2080_Pin_Configuration_Level_Sensitive};
	HDC2080_Interrupts_t interrupt_cfg = {
		.dataready = HDC2080_Interrupt_State_Disabled,
		.temperature_thr_high = HDC2080_Interrupt_State_Disabled,
		.temperature_thr_low = HDC2080_Interrupt_State_Disabled,
		.humidity_thr_high = HDC2080_Interrupt_State_Disabled,
		.humidity_thr_low = HDC2080_Interrupt_State_Disabled};
	float val;

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "intDataReadyEn"))
		{
	        interrupt_cfg.dataready = HDC2080_Interrupt_State_Enabled;
	        my_printf("intDataReadyEn OK\n");
		}
		else if (0U == strcmp(argv[i], "intTempMaxEn"))
		{
			interrupt_cfg.temperature_thr_high = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Temperature_Threshold_High(&hdc, val);
			if (ret == HDC2080_OK)
			{
				my_printf("intTempMaxEn OK, float: %.1f\n", val);
			}
			else
			{
				my_printf("intTempMaxEn ECODE: %d, float: %.1f\n", ret, val);
			}

		}
		else if (0U == strcmp(argv[i], "intTempMinEn"))
		{
			interrupt_cfg.temperature_thr_low = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Temperature_Threshold_Low(&hdc, val);
			if (ret == HDC2080_OK)
			{
				my_printf("intTempMinEn OK, float: %.1f\n", val);
			}
			else
			{
				my_printf("intTempMinEn ECODE: %d, float: %.1f\n", ret, val);
			}
		}
		else if (0U == strcmp(argv[i], "intHumMaxEn"))
		{
			interrupt_cfg.humidity_thr_high = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Humidity_Threshold_High(&hdc, val);
			if (ret == HDC2080_OK)
			{
				my_printf("intHumMaxEn OK, float: %.1f\n", val);
			}
			else
			{
				my_printf("intHumMaxEn ECODE: %d, float: %.1f\n", ret, val);
			}
		}
		else if (0U == strcmp(argv[i], "intHumMinEn"))
		{
			interrupt_cfg.humidity_thr_low = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Humidity_Threshold_Low(&hdc, val);
			if (ret == HDC2080_OK)
			{
				my_printf("intHumMinEn OK, float: %.1f\n", val);
			}
			else
			{
				my_printf("intHumMinEn ECODE: %d, float: %.1f\n", ret, val);
			}
		}
		else if (0U == strcmp(argv[i], "intDis"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_High_Z;
	        my_printf("intDis OK\n");
		}
		else if (0U == strcmp(argv[i], "intAL"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_Enable;
	        interrupt_pin.polarity = HDC2080_Pin_Configuration_Active_Low;
	        my_printf("intAL OK\n");
		}
		else if (0U == strcmp(argv[i], "intAH"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_Enable;
	        interrupt_pin.polarity = HDC2080_Pin_Configuration_Active_High;
	        my_printf("intAH OK\n");
		}
		else if (0U == strcmp(argv[i], "levelSens"))
		{
	        interrupt_pin.mode = HDC2080_Pin_Configuration_Level_Sensitive;
	        my_printf("levelSens OK\n");
		}
		else if (0U == strcmp(argv[i], "CompMode"))
		{
	        interrupt_pin.mode = HDC2080_Pin_Configuration_Comparator_Mode;
	        my_printf("CompMode OK\n");
		}
		else
		{
			my_printf("Bad argument: %s\n",argv[i]);
		}
	}

	ret = HDC2080_Set_INT_Pin_Configuration(&hdc, &interrupt_pin);
	my_printf("HDC2080_Set_INT_Pin_Configuration: %d\n", ret);
	ret = HDC2080_Set_Interrupt_Configuration(&hdc, &interrupt_cfg);
	my_printf("HDC2080_Set_Interrupt_Configuration: %d\n", ret);

	ret = HDC2080_Start_Conversion(&hdc);
	my_printf("HDC2080_Start_Conversion: %d\n", ret);
}


static void HDC_CLI_Cmd_Deinit(int argc, char **argv)
{
	HDC2080_Status_t ret;
	ret = HDC2080_Soft_Reset(&hdc);
	my_printf("HDC2080_Soft_Reset: %d\n", ret);
}


static void HDC_CLI_Cmd_Force_MEasurement(int argc, char **argv)
{
	HDC2080_Status_t ret;
	ret = HDC2080_Start_Conversion(&hdc);
	if(ret == HDC2080_OK)
	{
		my_printf("Measurement Forced OK\n");
	}
	else
	{
		my_printf("Measurement Forced ECODE: %d\n", ret);
	}
}

static void HDC_hex_to_offset_temp(uint8_t hex, HDC2080_Temperature_Offset_t *offset)
{
	if ((hex & (1 << 0)) != 0) {
		offset->of_0_16 = 1;
	}
	if ((hex & (1 << 1)) != 0) {
		offset->of_0_32 = 1;
	}
	if ((hex & (1 << 2)) != 0) {
		offset->of_0_64 = 1;
	}
	if ((hex & (1 << 3)) != 0) {
		offset->of_1_28 = 1;
	}
	if ((hex & (1 << 4)) != 0) {
		offset->of_2_58 = 1;
	}
	if ((hex & (1 << 5)) != 0) {
		offset->of_5_16 = 1;
	}
	if ((hex & (1 << 6)) != 0) {
		offset->of_10_32 = 1;
	}
	if ((hex & (1 << 7)) != 0) {
		offset->of_neg_20_32 = 1;
	}
}

static void HDC_hex_to_offset_hum(uint8_t hex, HDC2080_Humidity_Offset_t *offset)
{
	if ((hex & (1 << 0)) != 0) {
		offset->of_0_2 = 1;
	}
	if ((hex & (1 << 1)) != 0) {
		offset->of_0_4 = 1;
	}
	if ((hex & (1 << 2)) != 0) {
		offset->of_0_8 = 1;
	}
	if ((hex & (1 << 3)) != 0) {
		offset->of_1_6 = 1;
	}
	if ((hex & (1 << 4)) != 0) {
		offset->of_3_1 = 1;
	}
	if ((hex & (1 << 5)) != 0) {
		offset->of_6_3 = 1;
	}
	if ((hex & (1 << 6)) != 0) {
		offset->of_12_5 = 1;
	}
	if ((hex & (1 << 7)) != 0) {
		offset->of_neg_25_0 = 1;
	}
}


