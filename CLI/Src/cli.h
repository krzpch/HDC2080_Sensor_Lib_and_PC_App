#ifndef _CMD_H_
#define _CMD_H_

/**********************************************************************************************
* Includes
***********************************************************************************************/

#include <stddef.h>
#include <stdint.h>

/**********************************************************************************************
* Module Preprocessor defines
***********************************************************************************************/

// #define CLI_PROCESS_CMD_IMMEDIATELY
#define CLI_ONE_COMMAND_MAX_SIZE    (64U)
#define CLI_MAX_ARGS                (8U)
#define CLI_COMMAND_END_CHAR        ('\n')

/**********************************************************************************************
* Module Preprocessor Macros
***********************************************************************************************/

/**********************************************************************************************
* Module Typedefs
***********************************************************************************************/

typedef void (*CLI_Func_Cmd_Handler_t)(int argc, char **argv);
typedef void (*CLI_Func_Print_t)(char *c);

typedef enum
{
    CLI_OK = 0,
    CLI_ERROR = 1,
} CLI_Status_t;

typedef enum
{
    CLI_CMD_PROCESSED,
    CLI_CMD_NOT_PENDING,
    CLI_CMD_NOT_FOUND
} CLI_State_t;

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
    CLI_Func_Print_t func_print;

    uint8_t cmd_pending_nb;
    CLI_Cmd_t *commands;
    size_t commands_nb;
} CLI_t;

/**********************************************************************************************
* Global Function Prototypes
***********************************************************************************************/

void CLI_Init(CLI_t *cli);
void CLI_Print(CLI_t *cli, char *msg);
CLI_State_t CLI_Process_Comand(CLI_t *cli);
CLI_Status_t CLI_Receive_Char(CLI_t *cli, char c);

/**********************************************************************************************
* Exported variables
***********************************************************************************************/

#endif
