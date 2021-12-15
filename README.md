dependencies: bison, yacc, flex, readline ??

1.  Functions parameters are returned by the parser in the form of an array. Beisides, the command
    is usually not provided using its full path, but instead works with a PATH variable like in
    other shells. The most appropriate command is thus execvp (v: array-shaped parameter list;
    p: path use, in our case imported from the shell that called ours).

2.  The sequence operator is denoted by ';' in Bash. the following commands behave differently
    (given that the user does not have write permissions in directory /a):
    1> touch /a/file && shutdown now
    2> touch /a/file; shutdown now
    indeed, in the first case the user will still be able to use its computer.

3.  parentheses form a "subshell". They require the shell to fork once in order to execute what is
    between the parentheses. An apparent result can be that the command '(cd /folder)' has no effect
    on the shell session. Another one is that the command '(cmd1; cmd2) > log' gets both commands
    outputs redirected in file 'log', whereas 'cmd1; cmd2 > log' is usually parsed as "execute cmd1,
    then write cmd2 output to log".
