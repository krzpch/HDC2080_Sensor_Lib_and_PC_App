#include "cli_hdc2080.h"
#include "HDC2080.h"
#include "usart.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

#define UART_HANDLER    (&huart2)
extern HDC2080_t hdc;

#define CHAR_BUFF_SIZE  (12U)

static int HDC_CLI_Print(char *c);
static int HDC_CLI_Cmd_Get_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Get_Humidity(int argc, char **argv);
static int HDC_CLI_Cmd_Get_Measurements(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Humidity(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Interrupts(int argc, char **argv);
static int HDC_CLI_Cmd_Init(int argc, char **argv);
static int HDC_CLI_Cmd_Deinit(int argc, char **argv);

static CLI_Cmd_t HDC2080_Cmds[] =
{
    {.command = "get_temperature",		.cmd_handler = &HDC_CLI_Cmd_Get_Temperature},
    {.command = "get_humidity",			.cmd_handler = &HDC_CLI_Cmd_Get_Humidity},
	{.command = "get_measurements",		.cmd_handler = &HDC_CLI_Cmd_Get_Measurements},
    {.command = "config_temperature",	.cmd_handler = &HDC_CLI_Cmd_Config_Temperature},
    {.command = "config_humidity",		.cmd_handler = &HDC_CLI_Cmd_Config_Humidity},
	{.command = "config_interrupts",	.cmd_handler = &HDC_CLI_Cmd_Config_Interrupts},
	{.command = "sensor_initialize",	.cmd_handler = &HDC_CLI_Cmd_Init},
	{.command = "sensor_deinitialize",	.cmd_handler = &HDC_CLI_Cmd_Deinit}
};

CLI_t HDC2080_CLI =
{
    .func_print_char = &HDC_CLI_Print,
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
	HDC2080_Start_Conversion(&hdc);
	HAL_Delay(1000);
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
	HDC2080_Start_Conversion(&hdc);
	HAL_Delay(1000);
	ret = HDC2080_Get_Humidity(&hdc, &hum);
	if(ret == HDC2080_OK)
	{
		printf("HUM %.1f\n", hum);
	}
	else
	{
		printf("Bad Hum Meas, ECODE: %d\n", ret);
	}
    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Measurements(int argc, char **argv)
{
    //TODO - temporary solution for testing
	float temp;
	float hum;
    //static int i = 0;
    //char *output[] = {"TEMP 24.5\nHUM 23.5\n", "TEMP 25.5\nHUM 24.5\n", "TEMP 26.5\nHUM 25.5\n", "TEMP 27.5\nHUM 26.5\n", "TEMP 28.3\nHUM 27.5\n",
    //                  "TEMP 22.8\nHUM 28.5\n", "TEMP 17.8\nHUM 29.5\n", "TEMP 15.2\nHUM 30.5\n", "TEMP 19.0\nHUM 31.5\n", "TEMP 21.4\nHUM 32.5\n"};
    //HAL_UART_Transmit(UART_HANDLER, output[i], strlen(output[i]), 100U);
    //i = (i + 1) % 10;

	HDC2080_Get_Temperature_Humidity(&hdc, &temp, &hum);
	printf("TEMP %.1f\nHUM %.1f\n", temp, hum);
    return 1; //TODO
}

static int HDC_CLI_Cmd_Config_Temperature(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "res9bit"))
		{
			char output[] = {"9bit_OK\n"};
			HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
			HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Humidity_Resolution_9_bit);
		}
		else if (0U == strcmp(argv[i], "res11bit"))
		{
			char output[] = {"11bit_OK\n"};
			HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
			HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Humidity_Resolution_11_bit);
		}
		else if (0U == strcmp(argv[i], "res14bit"))
		{
			char output[] = {"14bit_OK\n"};
			HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
			HDC2080_Set_Temperature_Resolution(&hdc, HDC2080_Humidity_Resolution_14_bit);
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
	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "res9bit"))
		{
			char output[] = {"9bit_OK\n"};
			HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
			HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Humidity_Resolution_9_bit);
		}
		else if (0U == strcmp(argv[i], "res11bit"))
		{
			char output[] = {"11bit_OK\n"};
			HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
			HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Humidity_Resolution_11_bit);
		}
		else if (0U == strcmp(argv[i], "res14bit"))
		{
			char output[] = {"14bit_OK\n"};
			HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
			HDC2080_Set_Humidity_Resolution(&hdc, HDC2080_Humidity_Resolution_14_bit);
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
	HDC2080_Soft_Reset(&hdc);

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "temp&hum"))
		{
	        printf("temp&hum OK\n");

	        HDC2080_Set_Measurement_Configuration(&hdc, HDC2080_Measurement_Configuration_Humidity_Temperature);
		}
		else if (0U == strcmp(argv[i], "onlyTemp"))
		{
	        char output[] = {"onlyTemp_OK\n"};
	        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);

	        HDC2080_Set_Measurement_Configuration(&hdc, HDC2080_Measurement_Configuration_Temperature);
		}
		else if (0U == strcmp(argv[i], "autoOff"))
		{
	        char output[] = {"autoOff_OK\n"};
	        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);

	        HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1_Disabled);
		}
		else if (0U == strcmp(argv[i], "1Hz"))
		{
	        char output[] = {"1Hz_OK\n"};
	        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);

	        HDC2080_Set_AMM_Rate(&hdc, HDC2080_AMM_Rate_1Hz);
		}
		else
		{
			printf("Bad argument: %s\n",argv[i]);
		}
	}
	printf("Init ok\n");
	return 1;
}

static int HDC_CLI_Cmd_Config_Interrupts(int argc, char **argv)
{
	HDC2080_INT_Pin_Configuration_t interrupt_pin = {0};
	HDC2080_Interrupts_t interrupt_cfg = {0};

	for (int i = 1; i < argc; i++)
	{
		if (0U == strcmp(argv[i], "intDataReadyDis"))
		{
	        interrupt_cfg.dataready = HDC2080_Interrupt_State_Disabled;

		}
		else if (0U == strcmp(argv[i], "intDataReadyEn"))
		{
			interrupt_cfg.dataready = HDC2080_Interrupt_State_Enabled;
		}
		else if (0U == strcmp(argv[i], "intDis"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_High_Z;
		}
		else if (0U == strcmp(argv[i], "intAL"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_Enable;
	        interrupt_pin.polarity = HDC2080_Pin_Configuration_Active_Low;
		}
		else if (0U == strcmp(argv[i], "intAH"))
		{
	        interrupt_pin.pin = HDC2080_Pin_Configuration_Enable;
	        interrupt_pin.polarity = HDC2080_Pin_Configuration_Active_High;
		}
		else if (0U == strcmp(argv[i], "levelSens"))
		{
	        interrupt_pin.mode = HDC2080_Pin_Configuration_Level_Sensitive;
		}
		else if (0U == strcmp(argv[i], "CompMode"))
		{
	        interrupt_pin.mode = HDC2080_Pin_Configuration_Comparator_Mode;
		}
		else
		{
			printf("Bad argument: %s\n",argv[i]);
		}
	}

	printf("Interrupt init\n");
	HDC2080_Set_INT_Pin_Configuration(&hdc, &interrupt_pin);
	HDC2080_Set_Interrupt_Configuration(&hdc, &interrupt_cfg);
	HDC2080_Start_Conversion(&hdc);
	return 1;
}


static int HDC_CLI_Cmd_Deinit(int argc, char **argv)
{
	HDC2080_Soft_Reset(&hdc);
}
