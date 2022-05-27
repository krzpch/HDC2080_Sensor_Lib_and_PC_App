#ifndef _CMD_H_
#define _CMD_H_

#include <stddef.h>
#include <stdint.h>

// #define CLI_PROCESS_CMD_IMMEDIATELY
#define CLI_ONE_COMMAND_MAX_SIZE    (64U)
#define CLI_COMMAND_END_CHAR        ('\n')

typedef enum
{
    CLI_OK = 0,
    CLI_ERROR = 1,
} CLI_Status_t;

typedef enum
{
    CLI_IDLE,
    CLI_CMD_PENDING
} CLI_State_t;

typedef int (*CLI_Func_Cmd_Handler_t)(int argc, char **argv);
typedef int (*CLI_Func_Print_Char_t)(const char *c);

typedef struct
{
    const char *command;
    CLI_Func_Cmd_Handler_t cmd_handler;
} CLI_Cmd_t;

typedef struct 
{
    char *buffer;
    size_t head;
    size_t tail;
    size_t size;
} CLI_Queue_t;

typedef struct
{
    CLI_Queue_t queue;
    CLI_Func_Print_Char_t func_print_char;

    uint8_t cmd_pending_nb;
    CLI_Cmd_t *commands;
    size_t commands_nb;
} CLI_t;

void CLI_Init(CLI_t *cli);
void CLI_Print(CLI_t *cli, const char *msg);
void CLI_Process_Comand(CLI_t *cli);
CLI_Status_t CLI_Receive_Char(CLI_t *cli, char c);

#endif
