/**********************************************************************************************
* Includes
***********************************************************************************************/
#include "cli.h"
#include <string.h>
#include <stdbool.h>

/**********************************************************************************************
* Module Preprocessor defines
***********************************************************************************************/

/**********************************************************************************************
* Module Preprocessor Macros
***********************************************************************************************/
#define CLI_IS_QUEUE_EMPTY(cli)         ((cli)->head == (cli)->tail)
#define CLI_IS_QUEUE_FULL(cli)          (((cli)->head + 1) % (cli)->size == (cli)->tail)

/**********************************************************************************************
* Module Typedefs
***********************************************************************************************/

/**********************************************************************************************
* Function Prototypes
***********************************************************************************************/

static CLI_Func_Cmd_Handler_t CLI_Find_Command(CLI_t *cli, const char *cmd);
static void CLI_Queue_Init(CLI_Queue_t *queue);
static bool CLI_Queue_Push(CLI_Queue_t *queue, char c);
static bool CLI_Queue_Pop(CLI_Queue_t *queue, char *c);

/**********************************************************************************************
* Module Variable Definitions
***********************************************************************************************/

/**********************************************************************************************
* Function Definitions
***********************************************************************************************/

static void CLI_Queue_Init(CLI_Queue_t *queue)
{
    queue->head = 0;
    queue->tail = 0;
}

static bool CLI_Queue_Push(CLI_Queue_t *queue, char c)
{
    if (CLI_IS_QUEUE_FULL(queue))
    {
        return false;
    }
    queue->buffer[queue->head] = c;
    queue->head = (queue->head + 1) % queue->size;

    return true;
}

static bool CLI_Queue_Pop(CLI_Queue_t *queue, char *c)
{
    if (CLI_IS_QUEUE_EMPTY(queue))
    {
        return false;
    }
    *c = queue->buffer[queue->tail];
    queue->tail = (queue->tail + 1) % queue->size;

    return true;
}

static CLI_Func_Cmd_Handler_t CLI_Find_Command(CLI_t *cli, const char *cmd)
{
    CLI_Func_Cmd_Handler_t command_func = NULL;

    for (size_t i = 0; i < cli->commands_nb; i++)
    {
        if (0 == strcmp(cli->commands[i].command, cmd))
        {
            command_func = cli->commands[i].cmd_handler;
            break;
        }
    }

    return (command_func);
}

void CLI_Print(CLI_t *cli, char *msg)
{
    for (char *c = msg; *c != '\0'; c++)
    {
        cli->func_print(c);
    }
}

void CLI_Init(CLI_t *cli)
{
    CLI_Queue_Init((CLI_Queue_t *)&cli->queue);
    cli->cmd_pending_nb = 0;
}

CLI_State_t CLI_Process_Comand(CLI_t *cli)
{
    uint8_t argc = 0;
    uint16_t cmd_len_cnt = 0;
    char c = '\0';
    char *argv[CLI_MAX_ARGS] = {0};
    char actual_cmd[CLI_ONE_COMMAND_MAX_SIZE] = {0};
    CLI_State_t state = CLI_CMD_NOT_PENDING;

    if (0 < cli->cmd_pending_nb) /* Check if cmd is pending */
    {
        state = CLI_CMD_PROCESSED;
        do
        {
            /* Read a command from queue */
            CLI_Queue_Pop((CLI_Queue_t *)&cli->queue, &c);
            actual_cmd[cmd_len_cnt++] = c;
        } while ((c != '\0') &&
                 (!CLI_IS_QUEUE_EMPTY(&cli->queue)) &&
                 (cmd_len_cnt < CLI_ONE_COMMAND_MAX_SIZE - 1));

        actual_cmd[CLI_ONE_COMMAND_MAX_SIZE] = '\0';
        argv[argc] = strtok(actual_cmd, " ");

        while ((argv[argc] != NULL) &&
               (argc < CLI_MAX_ARGS))
        {
            /* Parse arguments */
            argv[++argc] = strtok(NULL, " ");
        }

        CLI_Func_Cmd_Handler_t command_func = CLI_Find_Command(cli, argv[0]);
        if (command_func != NULL)
        {
            command_func(argc, argv);
        }
        else
        {
            char *cmd_not_found = "Command Not Found\n";
            CLI_Print(cli, cmd_not_found);
            state = CLI_CMD_NOT_FOUND;
        }

        cli->cmd_pending_nb -= 1;
    }

    return state;
}

CLI_Status_t CLI_Receive_Char(CLI_t *cli, char c)
{
    bool push_status = false;

    if (c == CLI_COMMAND_END_CHAR)
    {
        if (true == CLI_Queue_Push((CLI_Queue_t *)&cli->queue, '\0'))
        {
            /* end of command, increment cmd pending number */
            cli->cmd_pending_nb += 1;
        }
        else
        {
            /* push_status already set to false */
        }
#ifdef CLI_PROCESS_CMD_IMMEDIATELY
        CLI_Process_Comand(cli);
#endif
    }
    else
    {
        /* push char into queue */
        push_status = CLI_Queue_Push((CLI_Queue_t *)&cli->queue, c);

        if(false == push_status)
        {
            if (0 == cli->cmd_pending_nb)
            {
                /* no command pending and cli buffer is full - reset CLI */
                CLI_Init(cli);
                push_status = CLI_Queue_Push((CLI_Queue_t *)&cli->queue, c);
            }
        }
    }

    return (push_status ? CLI_OK : CLI_ERROR);
}