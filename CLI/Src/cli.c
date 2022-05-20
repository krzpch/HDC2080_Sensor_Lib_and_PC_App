#include "cli.h"
#include <string.h>

#define CLI_COMMAND_END_CHAR '\n'
#define CLI_MAX_ARGS (8U)

#define CLI_TRUE (1U)
#define CLI_FALSE (0U)

#define CLI_IS_BUFFER_FULL(cli) (((cli)->buff_ptr - (cli)->rx_buffer) >= CLI_BUFFER_SIZE)

static void CLI_Print(CLI_t *cli, const char *msg);
static CLI_Func_Cmd_Handler_t CLI_Find_Command(CLI_t *cli, const char *cmd);

static void CLI_Print(CLI_t *cli, const char *msg)
{
    for (const char *c = msg; *c != '\0'; c++)
    {
        cli->func_print_char(c);
    }
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

void CLI_Init(CLI_t *cli)
{
    cli->buff_ptr = cli->rx_buffer;
    cli->is_cmd_pending = CLI_FALSE;
}

void CLI_Process_Comand(CLI_t *cli)
{
    uint8_t argc = 0;
    char *argv[CLI_MAX_ARGS] = {0};

    if (CLI_TRUE == cli->is_cmd_pending)
    {
        argv[argc] = strtok(cli->rx_buffer, " ");
        while ((argv[argc] != NULL) &&
               (argc < CLI_MAX_ARGS))
        {
            argv[++argc] = strtok(NULL, " ");
        }

        CLI_Func_Cmd_Handler_t command_func = CLI_Find_Command(cli, argv[0]);
        if (command_func != NULL)
        {
            command_func(argc, argv);
        }
        else
        {
            // TODO: cmd not found string in cli_t struct
            char *cmd_not_found = "Command Not Found\n";
            CLI_Print(cli, cmd_not_found);
            CLI_Print(cli, argv[0]);
            CLI_Print(cli, "\n");
        }

        cli->is_cmd_pending = CLI_FALSE;
    }

    // TODO: return
}

void CLI_Receive_Char(CLI_t *cli, char c)
{
    if (c == CLI_COMMAND_END_CHAR)
    {
        if (CLI_FALSE == cli->is_cmd_pending)
        {
            *(cli->buff_ptr) = '\0';
            cli->buff_ptr = cli->rx_buffer;
            cli->is_cmd_pending = CLI_TRUE;
#ifdef CLI_PROCESS_CMD_IMMEDIATELY
            CLI_Process_Comand(cli);
#endif
        }
        else
        {
            // TODO: return error
        }
    }

    else
    {
        if (CLI_FALSE == CLI_IS_BUFFER_FULL(cli))
        {
            *cli->buff_ptr++ = c;
        }
        else
        {
            // TODO: return error
        }
    }
}
