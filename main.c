#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <wordexp.h>

#include "global.h"

#define NAME "SUPER SHELL 3000"


void errmsg (char *msg)
{
    fprintf(stderr, "error: %s\n", msg);
}

// In bash, only the last output redirection is applied if several are provided
// Here, append (>>) has precedence over (>), regardless of other redirections

void apply_redirects (struct cmd *cmd)
{
    if (cmd->input)
    {
	int fd = open(cmd->input, O_RDONLY, 0666);
	if (fd == -1)
	{
	    perror(cmd->input);
	    exit(errno);
	}
	dup2(fd, 0);
    }
    if (!cmd->append && cmd->output)
    {
	int fd = creat(cmd->output, 0666);
	if (fd == -1)
	{
	    perror(cmd->output);
	    exit(errno);
	}
	dup2(fd, 1);
    }
    if (cmd->append)
    {
	int fd = open(cmd->append, O_CREAT | O_WRONLY | O_APPEND, 0666);
	if (fd == -1)
	{
	    perror(cmd->append);
	    exit(errno);
	}
	dup2(fd, 1);
    }
    if (cmd->error)
    {
	int fd = creat(cmd->error, 0666);
	if (fd == -1)
	{
	    perror(cmd->error);
	    exit(errno);
	}
	dup2(fd, 2);
    }
}

// detects if the given C_PLAIN command is a shell builtin, and execute it as such if it is the case.
int exec_builtin (struct cmd *cmd)
{
    if (strcmp(cmd->args[0], "cd") == 0)
    {
	char *dest = cmd->args[1] ? cmd->args[1] : getenv("HOME");
	
	if (chdir(dest))
	{
	    perror(dest);
	    return errno;
	}
	     
	char *dir_path = calloc(FILENAME_MAX, sizeof(char));
	getcwd(dir_path, FILENAME_MAX);
	setenv("PWD", dir_path, 1);
	free(dir_path);
	return errno;
    }
    else
	return -1;
}

// The function execute() takes a command parsed at the command line.
// The structure of the command is explained in output.c.
// Returns the exit code of the command in question.
// The apply_redirects function is only applied in the C_PLAIN and C_VOID cases

int execute (struct cmd *cmd)
{
    switch (cmd->type)
    {
	case C_PLAIN:
	    {
		int ret = exec_builtin(cmd);
		if (ret != -1) return ret;

		wordexp_t *expansion = malloc(sizeof(wordexp_t));
		expansion->we_wordc = 0;

		for (int i = 0; cmd->args[i] != NULL; i++)
		{ // expand the arguments
		    int status = wordexp(cmd->args[i], expansion, WRDE_APPEND);
		    if (status) return status;
		}

		int pid = fork();
		if (pid < 0) return 1;

		if (pid == 0)
		{
		    apply_redirects(cmd);
		    execvp(expansion->we_wordv[0], expansion->we_wordv);
		    perror(expansion->we_wordv[0]);
		    exit(errno);
		}

		wordfree(expansion);
		free(expansion);

		int status;
		waitpid(pid, &status, 0);
		return status;
	    }

	case C_SEQ:
	    {
		execute(cmd->left);
		return execute(cmd->right);
	    }

	case C_AND:
	    {
		int ret = execute(cmd->left);
		if (!ret)
		    return execute(cmd->right);
		else
		    return ret;
	    }

	case C_OR:
	    {
		int ret = execute(cmd->left);
		if (ret)
		    return execute(cmd->right);
		else
		    return ret;
	    }

	case C_PIPE:
	    {
		int p_descriptor[2];
		pipe(p_descriptor);

		int lf = fork();
		if (lf == 0)
		{
		    dup2(p_descriptor[1], 1); close(p_descriptor[0]);
		    execute(cmd->left);
		    close(p_descriptor[1]);
		    exit(errno);
		}

		int rf = fork();
		if (rf == 0)
		{
		    dup2(p_descriptor[0], 0); close(p_descriptor[1]);
		    execute(cmd->right);
		    close(p_descriptor[0]);
		    exit(errno);
		}

		close(p_descriptor[0]); close(p_descriptor[1]);

		int status1, status2;
		waitpid(-1, &status1, 0);
		waitpid(-1, &status2, 0);
		if (status1 == 0)
		{
		    return status2;
		}
		else return status1;
	    }


	case C_VOID:
	    {
		int pid = fork();
		if (pid < 0) return 1;

		if (pid == 0)
		{
		    apply_redirects(cmd);
		    execute(cmd->left);
		    exit(errno);
		}

		int status;
		waitpid(pid, &status, 0);
		return status;
	    }

	default: // cannot happen
	    return -1;
    }
}

static void sig_handler (int signo)
{}

int main (int argc, char **argv)
{
    struct sigaction psa;
    psa.sa_handler = sig_handler;
    sigaction(SIGINT, &psa, NULL);

    int res = 0;
    char *good = ":) > ";
    char *bad  = ":( > ";
    char *prompt = good;
    printf("welcome to %s!\n", NAME);

    while (1)
    {
	printf("\n%s in %s:\n", getenv("USER"), getenv("PWD"));
	char *line = readline(prompt);
	if (!line) break;	// user pressed Ctrl+D; quit shell
	if (!*line) continue;	// empty line

	add_history(line);	// add line to history

	struct cmd *cmd = parser(line);
	if (!cmd) continue;	// some parse error occurred; ignore
	res = execute(cmd);
	prompt = res ? bad : good;
    }

    printf("\ngoodbye!\n");
    return EXIT_SUCCESS;
}
