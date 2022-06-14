#include "cli_hdc2080.h"
#include "usart.h"
#include <string.h>

#if (TARGET == HDC2080_F407)
#define UART_HANDLER    (&huart4)
#endif

#define CHAR_BUFF_SIZE  (12U)
#define HDC2080_CLI_BUFFER_SIZE (512U)

static int HDC_CLI_Print(char *c);
static int HDC_CLI_Cmd_Get_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Get_Humidity(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Temperature(int argc, char **argv);
static int HDC_CLI_Cmd_Config_Humidity(int argc, char **argv);

static CLI_Cmd_t HDC2080_Cmds[] =
{
    {.command = "get_temperature",     .cmd_handler = &HDC_CLI_Cmd_Get_Temperature},
    {.command = "get_humidity",        .cmd_handler = &HDC_CLI_Cmd_Get_Humidity},
    {.command = "config_temperature",  .cmd_handler = &HDC_CLI_Cmd_Config_Temperature},
    {.command = "config_humidity",     .cmd_handler = &HDC_CLI_Cmd_Config_Humidity}
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
    HAL_UART_Transmit(UART_HANDLER, c, 1U, 10U);
    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Temperature(int argc, char **argv)
{
    //TODO - temporary solution for testing
    static int i = 0;
    char *output[] = {"23.5\n\r", "24.5\n\r", "25.5\n\r", "26.5\n\r", "27.5\n\r",
                      "28.5\n\r", "29.5\n\r", "30.5\n\r", "31.5\n\r", "32.5\n\r"};
    HAL_UART_Transmit(UART_HANDLER, output[i], strlen(output[i]), 10U);
    i = i++ % 10;
    return 1; //TODO
}

static int HDC_CLI_Cmd_Get_Humidity(int argc, char **argv)
{
    //TODO - temporary solution for testing
    static int i = 0;
    char *output[] = {"23.5\n\r", "24.5\n\r", "25.5\n\r", "26.5\n\r", "27.5\n\r",
                      "28.5\n\r", "29.5\n\r", "30.5\n\r", "31.5\n\r", "32.5\n\r"};
    HAL_UART_Transmit(UART_HANDLER, output[i], strlen(output[i]), 10U);
    i = i++ % 10;
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