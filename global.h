typedef enum {
    C_PLAIN,	// string (regular command)
    C_VOID,	// subshell execution
    C_AND,	// &&
    C_OR,	// ||
    C_PIPE,	// regular UNIX pipe |
    C_SEQ	// sequence of commands ;
} cmdtype;

struct cmd {
	int type;
	struct cmd *left;
	struct cmd *right;

	char **args;
	char *input;
	char *output;
	char *append;
	char *error;
};

struct arglist {
	char *arg;
	struct arglist *next;
};

extern struct cmd* parser (char*);
extern void output (struct cmd*,int);
