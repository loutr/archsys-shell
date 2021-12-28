<p align="center">
  <img src="media/ENS_logo.png" alt="Logo ENS" height=80">
  <h2 align="center">ENS Paris-Saclay · DER Informatique</h2>
  <h4 align="center">SUPER SHELL 3000</h3>
</p>

---
## About
This project was made following the "Architecture et Systèmes" lecture at the Department of
Computer Science of the ENS Paris-Saclay. Its goal is to provide a minimalist shell
(command-line interpreter). The subject can be found at `doc/guidelines.pdf`.

You may find the answers to the different questions asked in it below.


## Answers to the subject

1.  Function parameters are returned by the parser in the form of an array. Besides, the command
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
    between the parentheses. An apparent result can be that the command `(cd folder)` has no effect
    on the shell session. Another one is that the command `(cmd1; cmd2) > log` gets both commands
    outputs redirected in file `log`, whereas `cmd1; cmd2 > log` is usually parsed as "execute cmd1,
    then write cmd2 output to log".

5.  The `^C` command is commonly caught by the terminal emulator and sent to the current process as a
    SIGINT. In order to prevent our shell from closing abruptly the way it does by default, this
    signal may be appropriately handled using the `signal` or 'sigaction' C functions.

6.  Note: only the `C_PLAIN` and `C_VOID` types of commands can be parsed with an extra file redirection
    option. Thus, appropriate redirections are only applied in these cases (with the function
    `apply_redirections`).

7.  The `dup2` command cannot be used on its own because it is necessary to bridge to file
    descriptors that are defined in two separate processes.

## Bonus questions

1. `ls` and `cat` are GNU utilities and are not directly linked to the shell (even though it is still
possible to reimplement them as well); unlike `cd` which is a shell built-in. I reimplemented it using
standard functions like `setenv`, `getenv`, `chdir`, etc. The special function `exec_builtin` catches
any C_PLAIN command that is interpreted as a shell built-in. This allows for better maintainability,
should any new built-in be added.

2. The standard library function `wordexp` matches our need for globbing (and actually much more
things), and it is what I used in this project. In order to reimplement it, one would have to:
  - decompose the string as {prefix_folder, suffix};
  - move to the appropriate folder and seek all matching files (potentially in sub-directories);
  - perform the actual expansion in the string, namely allocating a new string
    with an appropriate size and adding the results of the previous search.

3. I did not implement `jobs`.

4. I did not implement variables. I believe an appropriate way to do it would be to change the lexer
and make it detect words of the form `<word1>=<word2>` and mark them separately. Some of these words may
not be variables declaration, like in `ls --color=auto`. Thus, the parser should be modified to detect that
and create a separate category, like `C_VAR`, that could be executed as a direct call to `setenv`.
No further expansion processing is required, because `wordexp` already handles it (as a consequence,
already-existing variables are expanded correctly without any additional work).

## Notes for myself
Unmatched characters in the lexer end up, *sooner or later* being displayed in the buffer, which is very
inconvenient and is why it is necessary to have the rule `. ;`. Besides, the definition of a word has to
differ from the original one (which is too restrictive and does not allow proper variable expansion),
without being too general like `[^\ ]*`, which leads to ill-formed commands like `cmd1; cmd2` into
`cmd1;`, `cmd2` (and not `cmd1`, `SEQ`, `cmd2` as it should be).
Because flex always try to match the longest string, the definition of a word must exclude `(`, `|`, etc.

## Usage

The shell asks successively for commands to be executed. The prompt of next command displays a smiley
that reflects the exit status of the previous one. The user may chain commands just like with `sh`:
`cmd1; cmd2`, `cmd1 | cmd2`, `cmd1 && cmd2`, etc.

The test folder (you may display it with `tree` to better see what is inside) can be used in order to
experiment with the wildcard and the `cd` built-in.

## Dependencies and installation

The executable can be compiled using the command `make`. Its dependencies are: `bison (debian: bison)`, `flex (debian: flex)`, `readline (debian: libreadline-dev)`.
