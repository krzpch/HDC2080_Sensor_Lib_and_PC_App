#ifndef _CMD_H_
#define _CMD_H_

#include <stddef.h>
#include <stdint.h>

// #define CLI_PROCESS_CMD_IMMEDIATELY
#define CLI_BUFFER_SIZE        (128U)

typedef int (*CLI_Func_Cmd_Handler_t)(int argc, char **argv);
typedef int (*CLI_Func_Print_Char_t)(char *c);

typedef struct
{
    const char *command;
    CLI_Func_Cmd_Handler_t cmd_handler;
} CLI_Cmd_t;

typedef struct
{
    volatile char *buff_ptr;
    volatile char rx_buffer[CLI_BUFFER_SIZE];
    CLI_Func_Print_Char_t func_print_char;

    uint8_t is_cmd_pending;
    CLI_Cmd_t *commands;
    size_t commands_nb;
} CLI_t;

void CLI_Init(CLI_t *cli);
void CLI_Process_Comand(CLI_t *cli);
void CLI_Receive_Char(CLI_t *cli, char c);

#endif
