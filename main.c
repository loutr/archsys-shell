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

#include "global.h"

// This is the file that you should work on.

// declaration
int execute (struct cmd *cmd);

// name of the program, to be printed in several places
#define NAME "SUPER SHELL 3000"

// Some helpful functions

void errmsg (char *msg)
{
	fprintf(stderr, "error: %s\n", msg);
}

// apply_redirects() should modify the file descriptors for standard
// input/output/error (0/1/2) of the current process to the files
// whose names are given in cmd->input/output/error.
// append is like output but the file should be extended rather
// than overwritten.

void apply_redirects (struct cmd *cmd)
{
	if (cmd->input || cmd->output || cmd->append || cmd->error)
	{
		errmsg("I do not know how to redirect, please help me!");
		exit(-1);
	}
}

// The function execute() takes a command parsed at the command line.
// The structure of the command is explained in output.c.
// Returns the exit code of the command in question.

int execute (struct cmd *cmd)
{
	switch (cmd->type)
	{
	    case C_PLAIN:
	    {
		int pid = fork();
		if (pid < 0) return 1;

		if (pid == 0)
		{
		    execvp(cmd->args[0], cmd->args);
		    printf("command not found: %s\n", cmd->args[0]);
		    exit(EXIT_FAILURE);
		}

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

	    }


	    case C_VOID:
		errmsg("I do not know how to do this, please help me!");
		return -1;
	}

	// Just to satisfy the compiler
	errmsg("This cannot happen!");
	return -1;
}

int main (int argc, char **argv)
{
	char *prompt = malloc(strlen(NAME) + 3);
	printf("welcome to %s!\n", NAME);
	sprintf(prompt, "%s> ", NAME);

	while (1)
	{
		char *line = readline(prompt);
		if (!line) break;	// user pressed Ctrl+D; quit shell
		if (!*line) continue;	// empty line

		add_history(line);	// add line to history

		struct cmd *cmd = parser(line);
		if (!cmd) continue;	// some parse error occurred; ignore
		//output(cmd, 0);	// activate this for debugging
		execute(cmd);
	}

	printf("goodbye!\n");
	return 0;
}
