#include "cli_hdc2080.h"
#include "HDC2080.h"
#include "usart.h"
#include "main.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define UART_HANDLER    (&huart2)
extern HDC2080_t hdc;

#define CHAR_BUFF_SIZE  (12U)
#define HDC2080_CLI_BUFFER_SIZE (512U)

static void HDC_hex_to_offset_temp(uint8_t hex, HDC2080_Temperature_Offset_t *offset);
static void HDC_hex_to_offset_hum(uint8_t hex, HDC2080_Humidity_Offset_t *offset);

static int HDC_CLI_Print(char *c);
static int HDC_CLI_Cmd_Get_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Get_Humidity(int argc, char **argv);
static int HDC_CLI_Cmd_Get_Measurements(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Humidity(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Interrupts(int argc, char **argv);
static int HDC_CLI_Cmd_Init(int argc, char **argv);
static int HDC_CLI_Cmd_Deinit(int argc, char **argv);
static int HDC_CLI_Cmd_Force_MEasurement(int argc, char **argv);

static CLI_Cmd_t HDC2080_Cmds[] =
{
    {.command = "get_temperature",		.cmd_handler = &HDC_CLI_Cmd_Get_Temperature},
    {.command = "get_humidity",			.cmd_handler = &HDC_CLI_Cmd_Get_Humidity},
	{.command = "get_measurements",		.cmd_handler = &HDC_CLI_Cmd_Get_Measurements},
    {.command = "config_temperature",	.cmd_handler = &HDC_CLI_Cmd_Config_Temperature},
    {.command = "config_humidity",		.cmd_handler = &HDC_CLI_Cmd_Config_Humidity},
	{.command = "config_interrupts",	.cmd_handler = &HDC_CLI_Cmd_Config_Interrupts},
	{.command = "sensor_initialize",	.cmd_handler = &HDC_CLI_Cmd_Init},
	{.command = "sensor_deinitialize",	.cmd_handler = &HDC_CLI_Cmd_Deinit},
	{.command = "force_measurement",	.cmd_handler = &HDC_CLI_Cmd_Force_MEasurement}

};

static char HDC2080_CLI_Rx_Buffer[HDC2080_CLI_BUFFER_SIZE] = {0};

CLI_t HDC2080_CLI =
{
    .queue = {.buffer = HDC2080_CLI_Rx_Buffer, .size = HDC2080_CLI_BUFFER_SIZE},
    .func_print = &HDC_CLI_Print,
    .commands = HDC2080_Cmds,
    .commands_nb = sizeof(HDC2080_Cmds) / sizeof(HDC2080_Cmds[0])
};

static int HDC_CLI_Print(char *c)
{
    HAL_UART_Transmit(UART_HANDLER, (uint8_t*)c, 1U, 10U);
    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Temperature(int argc, char **argv)
{
	float temp;
	HDC2080_Status_t ret;
	ret = HDC2080_Get_Temperature(&hdc, &temp);
	if(ret == HDC2080_OK)
	{
		printf("TEMP %.1f\n", temp);
	}
	else
	{
		printf("Bad Temp Meas, ECODE: %d\n", ret);
	}

    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Humidity(int argc, char **argv)
{
	float hum;
	HDC2080_Status_t ret;
	ret = HDC2080_Get_Humidity(&hdc, &hum);
	if (ret == HDC2080_OK) {
		printf("HUM %.1f\n", hum);
	} else {
		printf("Bad Hum Meas, ECODE: %d\n", ret);
	}
    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Measurements(int argc, char **argv)
{
	HDC2080_Status_t ret;
	float temp;
	float hum;

	ret = HDC2080_Get_Temperature_Humidity(&hdc, &temp, &hum);
	if (ret == HDC2080_OK) {
		printf("TEMP %.1f\nHUM %.1f\n", temp, hum);
	} else {
		printf("HDC2080_Get_Temperature_Humidity, ECODE: %d\n", ret);
	}
    return 1; //TODO
}

static int HDC_CLI_Cmd_Config_Temperature(int argc, char **argv)
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
				printf("res9bit OK\n");
			} else {
				printf("res9bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res11bit"))
		{
			ret = HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Resolution_11_bit);
			if (ret == HDC2080_OK) {
				printf("res11bit OK\n");
			} else {
				printf("res11bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res14bit"))
		{
			ret = HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Resolution_14_bit);
			if (ret == HDC2080_OK) {
				printf("res14bit OK\n");
			} else {
				printf("res14bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "temp_offset"))
		{
			offset = strtol(argv[++i], NULL, 16);
			HDC_hex_to_offset_temp(offset, &temp_offset);
			ret = HDC2080_Set_Temperature_Offset(&hdc, &temp_offset);

			if (ret == HDC2080_OK)
			{
				printf("temp_offset OK, offset: %x\n",offset);
			}
			else
			{
				printf("temp_offset ECODE: %d, offset: %x\n", ret, offset);
			}
		}
		else
		{
			printf("Bad argument: %s\n",argv[i]);
		}
	}

    return 1; // TODO
}

static int HDC_CLI_Cmd_Config_Humidity(int argc, char **argv)
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
				printf("res9bit OK\n");
			} else {
				printf("res9bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res11bit"))
		{
			ret = HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Resolution_11_bit);
			if (ret == HDC2080_OK) {
				printf("res11bit OK\n");
			} else {
				printf("res11bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "res14bit"))
		{
			ret = HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Resolution_14_bit);
			if (ret == HDC2080_OK) {
				printf("res14bit OK\n");
			} else {
				printf("res14bit ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "hum_offset"))
		{
			offset = strtol(argv[++i], NULL, 16);
			HDC_hex_to_offset_hum(offset, &temp_offset);
			ret = HDC2080_Set_Humidity_Offset(&hdc, &temp_offset);

			if (ret == HDC2080_OK)
			{
				printf("hum_offset OK, offset: %x\n",offset);
			}
			else
			{
				printf("hum_offset ECODE: %d, offset: %x\n", ret, offset);
			}
		}
		else
		{
			printf("Bad argument: %s\n",argv[i]);
		}
	}
    return 1;
}

static int HDC_CLI_Cmd_Init(int argc, char **argv)
{
	HDC2080_Status_t ret;
	ret = HDC2080_Soft_Reset(&hdc);
	if (ret == HDC2080_OK) {
		printf("softRes OK\n");
	} else {
		printf("softRes ECODE: %d\n", ret);
	}

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "temp&hum"))
		{
	        ret = HDC2080_Set_Measurement_Configuration(&hdc, HDC2080_Measurement_Configuration_Humidity_Temperature);
			if (ret == HDC2080_OK) {
				printf("temp&hum OK\n");
			} else {
				printf("temp&hum ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "onlyTemp"))
		{
	        ret = HDC2080_Set_Measurement_Configuration(&hdc, HDC2080_Measurement_Configuration_Temperature);
			if (ret == HDC2080_OK) {
				printf("onlyTemp OK\n");
			} else {
				printf("onlyTemp ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "autoOff"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1_Disabled);
			if (ret == HDC2080_OK) {
				printf("autoOff OK\n");
			} else {
				printf("autoOff ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "1/120Hz"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1_120Hz);
			if (ret == HDC2080_OK) {
				printf("120Hz OK\n");
			} else {
				printf("120Hz ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "1/10Hz"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1_10Hz);
			if (ret == HDC2080_OK) {
				printf("10Hz OK\n");
			} else {
				printf("10Hz ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "1Hz"))
		{
	        ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1Hz);
			if (ret == HDC2080_OK) {
				printf("1Hz OK\n");
			} else {
				printf("1Hz ECODE: %d\n", ret);
			}
		}
		else if (0U == strcmp(argv[i], "5Hz"))
		{
			ret = HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_5Hz);
			if (ret == HDC2080_OK) {
				printf("5Hz OK\n");
			} else {
				printf("5Hz ECODE: %d\n", ret);
			}
		}
		else
		{
			printf("Bad argument: %s\n",argv[i]);
		}
	}

	return 1;
}

static int HDC_CLI_Cmd_Config_Interrupts(int argc, char **argv)
{
	HDC2080_Status_t ret;
	HDC2080_INT_Pin_Configuration_t interrupt_pin = {0};
	HDC2080_Interrupts_t interrupt_cfg = {0};
	float val;

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "intDataReadyEn"))
		{
	        interrupt_cfg.dataready = HDC2080_Interrupt_State_Enabled;
	        printf("intDataReadyEn OK\n");
		}
		else if (0U == strcmp(argv[i], "intTempMaxEn"))
		{
			interrupt_cfg.temperature_thr_high = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Temperature_Threshold_High(&hdc, val);
			if (ret == HDC2080_OK)
			{
				printf("intTempMaxEn OK, float: %.1f\n", val);
			}
			else
			{
				printf("intTempMaxEn ECODE: %d, float: %.1f\n", ret, val);
			}

		}
		else if (0U == strcmp(argv[i], "intTempMinEn"))
		{
			interrupt_cfg.temperature_thr_low = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Temperature_Threshold_Low(&hdc, val);
			if (ret == HDC2080_OK)
			{
				printf("intTempMinEn OK, float: %.1f\n", val);
			}
			else
			{
				printf("intTempMinEn ECODE: %d, float: %.1f\n", ret, val);
			}
		}
		else if (0U == strcmp(argv[i], "intHumMaxEn"))
		{
			interrupt_cfg.humidity_thr_high = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Humidity_Threshold_High(&hdc, val);
			if (ret == HDC2080_OK)
			{
				printf("intHumMaxEn OK, float: %.1f\n", val);
			}
			else
			{
				printf("intHumMaxEn ECODE: %d, float: %.1f\n", ret, val);
			}
		}
		else if (0U == strcmp(argv[i], "intHumMinEn"))
		{
			interrupt_cfg.humidity_thr_low = HDC2080_Interrupt_State_Enabled;
			val = (float) atof(argv[++i]);
			ret = HDC2080_Set_Humidity_Threshold_Low(&hdc, val);
			if (ret == HDC2080_OK)
			{
				printf("intHumMinEn OK, float: %.1f\n", val);
			}
			else
			{
				printf("intHumMinEn ECODE: %d, float: %.1f\n", ret, val);
			}
		}
		else if (0U == strcmp(argv[i], "intDis"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_High_Z;
	        printf("intDis OK\n");
		}
		else if (0U == strcmp(argv[i], "intAL"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_Enable;
	        interrupt_pin.polarity = HDC2080_Pin_Configuration_Active_Low;
	        printf("intAL OK\n");
		}
		else if (0U == strcmp(argv[i], "intAH"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_Enable;
	        interrupt_pin.polarity = HDC2080_Pin_Configuration_Active_High;
	        printf("intAH OK\n");
		}
		else if (0U == strcmp(argv[i], "levelSens"))
		{
	        interrupt_pin.mode = HDC2080_Pin_Configuration_Level_Sensitive;
	        printf("levelSens OK\n");
		}
		else if (0U == strcmp(argv[i], "CompMode"))
		{
	        interrupt_pin.mode = HDC2080_Pin_Configuration_Comparator_Mode;
	        printf("CompMode OK\n");
		}
		else
		{
			printf("Bad argument: %s\n",argv[i]);
		}
	}

	ret = HDC2080_Set_INT_Pin_Configuration(&hdc, &interrupt_pin);
	printf("HDC2080_Set_INT_Pin_Configuration: %d\n", ret);
	ret = HDC2080_Set_Interrupt_Configuration(&hdc, &interrupt_cfg);
	printf("HDC2080_Set_Interrupt_Configuration: %d\n", ret);

	ret = HDC2080_Start_Conversion(&hdc);
	printf("HDC2080_Start_Conversion: %d\n", ret);

	return 1;
}


static int HDC_CLI_Cmd_Deinit(int argc, char **argv)
{
	HDC2080_Status_t ret;
	ret = HDC2080_Soft_Reset(&hdc);
	printf("HDC2080_Soft_Reset: %d\n", ret);
	return 1;
}


static int HDC_CLI_Cmd_Force_MEasurement(int argc, char **argv)
{
	HDC2080_Status_t ret;
	ret = HDC2080_Start_Conversion(&hdc);
	if(ret == HDC2080_OK)
	{
		printf("Measurement Forced OK\n");
	}
	else
	{
		printf("Measurement Forced ECODE: %d\n", ret);
	}
	return 1;
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


