/*
 * Adrianna Guevarra
 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/wait.h>
#include "BennySh.h"
#define BUFFERSIZE 100

unsigned short isVerbose = 0;

int main(int argc, char *argv[])
{
    char str[MAX_STR_LEN];
    char *ret_val;
    char *raw_cmd;
    cmd_list_t *cmd_list = NULL;
    int cmd_count = 0;
    char prompt[30];

    sprintf(prompt, PROMPT_STR " %s :-) ", getenv("LOGNAME"));
    for (;;)
    {
        fputs(prompt, stdout);
        memset(str, 0, MAX_STR_LEN);
        ret_val = fgets(str, MAX_STR_LEN, stdin);

        if (NULL == ret_val)
        {

            break;
        }

        str[strlen(str) - 1] = 0;
        if (strlen(str) == 0)
        {

            continue;
        }

        if (strcmp(str, EXIT_CMD) == 0)
        {

            break;
        }

        raw_cmd = strtok(str, PIPE_DELIM);

        cmd_list = (cmd_list_t *)calloc(1, sizeof(cmd_list_t));

        cmd_count = 0;
        while (raw_cmd != NULL)
        {
            cmd_t *cmd = (cmd_t *)calloc(1, sizeof(cmd_t));

            cmd->raw_cmd = strdup(raw_cmd);
            cmd->list_location = cmd_count++;

            if (cmd_list->head == NULL)
            {

                cmd_list->tail = cmd_list->head = cmd;
            }
            else
            {

                cmd_list->tail->next = cmd;
                cmd_list->tail = cmd;
            }
            cmd_list->count++;

            raw_cmd = strtok(NULL, PIPE_DELIM);
        }

        parse_commands(cmd_list);

        exec_commands(cmd_list);

        free_list(cmd_list);
        cmd_list = NULL;
    }

    return (EXIT_SUCCESS);
}

void simple_argv(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "hv")) != -1)
    {
        switch (opt)
        {
        case 'h':

            fprintf(stdout, "You must be out of your Vulcan mind if you think\n"
                            "I'm going to put helpful things in here.\n\n");
            exit(EXIT_SUCCESS);
            break;
        case 'v':

            isVerbose++;
            if (isVerbose)
            {
                fprintf(stderr, "verbose: verbose option selected: %d\n", isVerbose);
            }
            break;
        case '?':
            fprintf(stderr, "*** Unknown option used, ignoring. ***\n");
            break;
        default:
            fprintf(stderr, "*** Oops, something strange happened <%c> ... ignoring ...***\n", opt);
            break;
        }
    }
}
void exec_commands(cmd_list_t *cmds)
{
    int j;
    cmd_t *cmd = cmds->head;
    pid_t pid;
    int status;

    if (1 == cmds->count)
    {
        if (!cmd->cmd)
        {

            return;
        }
        if (0 == strcmp(cmd->cmd, CD_CMD))
        {
            if (0 == cmd->param_count)
            {

                char *home_dir = getenv("HOME");
                chdir(home_dir);
            }
            else
            {

                if (0 == chdir(cmd->param_list->param))
                {
                }
                else
                {
                }
            }
        }
        else if (0 == strcmp(cmd->cmd, PWD_CMD))
        {
            char str[MAXPATHLEN];

            getcwd(str, MAXPATHLEN);
            printf(" " PWD_CMD ": %s\n", str);
        }
        else if (0 == strcmp(cmd->cmd, ECHO_CMD))
        {

            param_t *node = cmd->param_list;
            while (node != NULL)
            {
                printf("%s ", node->param);
                node = node->next;
            }
            printf("\n");
        }
        else
        {
            char **args = calloc(cmd->param_count + 2, sizeof(char *));
            int counter = 1;
            param_t *node;
            node = cmd->param_list;
            args[0] = strdup(cmd->cmd);

            while (node != NULL)
            {
                args[counter] = strdup(node->param);
                node = node->next;
                counter++;
            }

            if ((pid = fork()) < 0)
            {
                printf("ERROR: forking child process failed\n");
                exit(1);
            }
            else if (pid == 0)
            {

                if (cmd->input_src == REDIRECT_FILE)
                {

                    int ifd = -1;

                    ifd = open(cmd->input_file_name, O_RDONLY);
                    dup2(ifd, STDIN_FILENO);
                    close(ifd);
                }
                if (cmd->output_dest == REDIRECT_FILE)
                {
                    int ifd = -1;
                    ifd = open(cmd->output_file_name, O_RDWR);
                    dup2(ifd, STDOUT_FILENO);
                    close(ifd);
                }

                if (execvp(args[0], args) < 0)
                {
                    printf("ERROR: exec failed\n");
                    exit(1);
                }
            }
            else
            {
                while (wait(&status) != pid)
                    ;
            }
            j = 0;
            for (; j < counter; j++)
            {
                free(args[j]);
            }
            free(args);
        }
    }
}

void free_list(cmd_list_t *cmd_list)
{
    cmd_t *tmp = NULL;
    cmd_t *cur = cmd_list->head;

    while (cur)
    {
        tmp = cur->next;

        free(cur->raw_cmd);
        free(cur->cmd);
        // param_list
        {
            param_t *p = cur->param_list;
            while (p)
            {
                param_t *tp = p->next;
                free(p->param);
                free(p);
                p = tp;
            }
        }

        free(cur->input_file_name);
        free(cur->output_file_name);

        free(cur);

        cur = tmp;
    }
    free(cmd_list);
}

void print_list(cmd_list_t *cmd_list)
{
    cmd_t *cmd = cmd_list->head;

    while (NULL != cmd)
    {
        print_cmd(cmd);
        cmd = cmd->next;
    }
}

void free_cmd(cmd_t *cmd)
{
}

void print_cmd(cmd_t *cmd)
{
    param_t *param = NULL;
    int pcount = 1;

    fprintf(stderr, "raw text: +%s+\n", cmd->raw_cmd);
    fprintf(stderr, "\tbase command: +%s+\n", cmd->cmd);
    fprintf(stderr, "\tparam count: %d\n", cmd->param_count);
    param = cmd->param_list;

    while (NULL != param)
    {
        fprintf(stderr, "\t\tparam %d: %s\n", pcount, param->param);
        param = param->next;
        pcount++;
    }

    fprintf(stderr, "\tinput source: %s\n", (cmd->input_src == REDIRECT_FILE ? "redirect file" : (cmd->input_src == REDIRECT_PIPE ? "redirect pipe" : "redirect none")));
    fprintf(stderr, "\toutput dest:  %s\n", (cmd->output_dest == REDIRECT_FILE ? "redirect file" : (cmd->output_dest == REDIRECT_PIPE ? "redirect pipe" : "redirect none")));
    fprintf(stderr, "\tinput file name:  %s\n", (NULL == cmd->input_file_name ? "<na>" : cmd->input_file_name));
    fprintf(stderr, "\toutput file name: %s\n", (NULL == cmd->output_file_name ? "<na>" : cmd->output_file_name));
    fprintf(stderr, "\tlocation in list of commands: %d\n", cmd->list_location);
    fprintf(stderr, "\n");
}

#define stralloca(_R, _S)              \
    {                                  \
        (_R) = alloca(strlen(_S) + 1); \
        strcpy(_R, _S);                \
    }

void parse_commands(cmd_list_t *cmd_list)
{
    cmd_t *cmd = cmd_list->head;
    char *arg;
    char *raw;

    while (cmd)
    {

        stralloca(raw, cmd->raw_cmd);

        arg = strtok(raw, SPACE_DELIM);
        if (NULL == arg)
        {

            cmd = cmd->next;
            continue;
        }

        if (arg[0] == '\'')
        {
            arg++;
        }
        if (arg[strlen(arg) - 1] == '\'')
        {
            arg[strlen(arg) - 1] = '\0';
        }
        cmd->cmd = strdup(arg);
        cmd->input_src = REDIRECT_NONE;
        cmd->output_dest = REDIRECT_NONE;

        while ((arg = strtok(NULL, SPACE_DELIM)) != NULL)
        {
            if (strcmp(arg, REDIR_IN) == 0)
            {

                cmd->input_file_name = strdup(strtok(NULL, SPACE_DELIM));
                cmd->input_src = REDIRECT_FILE;
            }
            else if (strcmp(arg, REDIR_OUT) == 0)
            {

                cmd->output_file_name = strdup(strtok(NULL, SPACE_DELIM));
                cmd->output_dest = REDIRECT_FILE;
            }
            else
            {

                param_t *param = (param_t *)calloc(1, sizeof(param_t));
                param_t *cparam = cmd->param_list;

                cmd->param_count++;

                if (arg[0] == '\'')
                {
                    arg++;
                }
                if (arg[strlen(arg) - 1] == '\'')
                {
                    arg[strlen(arg) - 1] = '\0';
                }
                param->param = strdup(arg);
                if (NULL == cparam)
                {
                    cmd->param_list = param;
                }
                else
                {

                    while (cparam->next != NULL)
                    {
                        cparam = cparam->next;
                    }
                    cparam->next = param;
                }
            }
        }

        if (cmd->list_location > 0)
        {
            cmd->input_src = REDIRECT_PIPE;
        }
        if (cmd->list_location < (cmd_list->count - 1))
        {
            cmd->output_dest = REDIRECT_PIPE;
        }

        cmd = cmd->next;
    }

    if (isVerbose > 0)
    {
        print_list(cmd_list);
    }
}
