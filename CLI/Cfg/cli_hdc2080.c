#include "cli_hdc2080.h"
#include "usart.h"
#include <string.h>

#define UART_HANDLER    (&huart2)


#define CHAR_BUFF_SIZE  (12U)

static int HDC_CLI_Print(char *c);
static int HDC_CLI_Cmd_Get_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Get_Humidity(int argc, char **argv);
static int HDC_CLI_Cmd_Get_Measurements(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Humidity(int argc, char **argv);

static CLI_Cmd_t HDC2080_Cmds[] =
{
    {.command = "get_temperature",		.cmd_handler = &HDC_CLI_Cmd_Get_Temperature},
    {.command = "get_humidity",			.cmd_handler = &HDC_CLI_Cmd_Get_Humidity},
    {.command = "config_temperature",	.cmd_handler = &HDC_CLI_Cmd_Config_Temperature},
	{.command = "get_measurements",		.cmd_handler = &HDC_CLI_Cmd_Get_Measurements},
    {.command = "config_humidity",		.cmd_handler = &HDC_CLI_Cmd_Config_Humidity}
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
    //TODO - temporary solution for testing
    static int i = 0;
    char *output[] = {"TEMP 23.5\n", "TEMP 24.5\n", "TEMP 25.5\n", "TEMP 26.5\n", "TEMP 27.5\n",
                      "TEMP 28.5\n", "TEMP 29.5\n", "TEMP 0.5\n", "TEMP 31.5\n", "TEMP 32.5\n"};
    HAL_UART_Transmit(UART_HANDLER, output[i], strlen(output[i]), 100U);
    i = (i + 1) % 10;
    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Humidity(int argc, char **argv)
{
    //TODO - temporary solution for testing
    static int i = 0;
    char *output[] = {"HUM 23.5\n", "HUM 24.5\n", "HUM 25.5\n", "HUM 26.5\n", "HUM 27.5\n",
                      "HUM 28.5\n", "HUM 29.5\n", "HUM 30.5\n", "HUM 31.5\n", "HUM 32.5\n"};
    HAL_UART_Transmit(UART_HANDLER, output[i], strlen(output[i]), 100U);
    i = (i + 1) % 10;
    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Measurements(int argc, char **argv)
{
    //TODO - temporary solution for testing
    static int i = 0;
    char *output[] = {"TEMP 24.5\nHUM 23.5\n", "TEMP 25.5\nHUM 24.5\n", "TEMP 26.5\nHUM 25.5\n", "TEMP 27.5\nHUM 26.5\n", "TEMP 28.3\nHUM 27.5\n",
                      "TEMP 22.8\nHUM 28.5\n", "TEMP 17.8\nHUM 29.5\n", "TEMP 15.2\nHUM 30.5\n", "TEMP 19.0\nHUM 31.5\n", "TEMP 21.4\nHUM 32.5\n"};
    HAL_UART_Transmit(UART_HANDLER, output[i], strlen(output[i]), 100U);
    i = (i + 1) % 10;
    return 1; //TODO
}

static int HDC_CLI_Cmd_Config_Temperature(int argc, char **argv)
{
    // TODO - temporary solution for testing
    if (0U == strcmp(argv[1], "resolution_9kbit"))
    {
        char output[] = {"9bit_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }
    else if (0U == strcmp(argv[1], "resolution_11kbit"))
    {
        char output[] = {"11bit_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }
    else if (0U == strcmp(argv[1], "resolution_14kbit"))
    {
        char output[] = {"14bit_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }
    else
    {
        char output[] = {"NOT_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }

    return 1; // TODO
}

static int HDC_CLI_Cmd_Config_Humidity(int argc, char **argv)
{
    // TODO - temporary solution for testing
    if (0U == strcmp(argv[1], "resolution_9kbit"))
    {
        char output[] = {"9bit_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }
    else if (0U == strcmp(argv[1], "resolution_11kbit"))
    {
        char output[] = {"11bit_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }
    else if (0U == strcmp(argv[1], "resolution_14kbit"))
    {
        char output[] = {"14bit_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }
    else
    {
        char output[] = {"NOT_OK\n\r"};
        HAL_UART_Transmit(UART_HANDLER, output, strlen(output), 10U);
    }
    return 1; // TODO
}
