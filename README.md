<p align="center">
  <img src="media/ENS_logo.png" alt="Logo ENS" height=80">
  <h2 align="center">ENS Paris-Saclay · DER Informatique</h2>
  <h4 align="center">My Little Shell</h3>
</p>

---
## About
This project was made following the "Architecture et Systèmes" lecture at the Department of
Computer Science of the ENS Paris-Saclay. Its goal is to provide a minimalist shell
(command-line interpreter). The subject can be found at `doc/guidelines.pdf`.

You may find the answers to the different questions asked in it below.


## Answers to the subject

1.  Functions parameters are returned by the parser in the form of an array. Besides, the command
    is usually not provided using its full path, but instead works with a PATH variable like in
    other shells. The most appropriate command is thus `execvp` (v: array-shaped parameter list;
    p: path use, in our case imported from the shell that called ours).

2.  The sequence operator is denoted by `;` in Bash. the following commands behave differently
    (given that the user does not have write permissions in directory /a):
    ```
    1> touch /a/file && shutdown now
    2> touch /a/file; shutdown now
    ```
    indeed, in the first case the user will still be able to use its computer.

4.  parentheses form a "subshell". They require the shell to fork once in order to execute what is
    between the parentheses. An apparent result can be that the command `(cd /folder)` has no effect
    on the shell session. Another one is that the command `(cmd1; cmd2) > log` gets both commands
    outputs redirected in file `log`, whereas `cmd1; cmd2 > log` is usually parsed as "execute cmd1,
    then write cmd2 output to log".

5.  The `^C` command is commonly caught by the terminal emulator and sent to the current process as a
    SIGINT. In order to prevent our shell from closing abruptly the way it does by default, this
    signal may be appropriately handled using the `signal` or 'sigaction' C functions.

6.  Note: only the `C_PLAIN` and `C_VOID` types of commands can be parsed with an extra file redirection
    option. Thus, appropriate redirections are only applied in these cases (with the function
    `apply_redirections`).

7.  The `dup2` command cannot be used because it is necessary to bridge to file descriptors that are
    defined in two separate processes.

## Bonus questions

`ls` and `cat` are GNU utilities and are not directly linked to the shell (even though it is still
possible to reimplement them as well); unlike `cd` which is a shell builtin.

## Dependencies and installation

The executable can be compiled using the command `make`. Its dependencies are: `bison (debian: bison)`, `flex (debian: flex)`, `readline (debian: libreadline-dev)`.
