# TechShell

## Group Members
- Edward Ware
- Cole Pellegrin
- Carter Landry

## Description
TechShell is a Unix-like shell written in C (specifically the C11 standard) using POSIX system calls.
It supports command execution, directory navigation, I/O redirection,
pipes, background execution, and detailed error handling.

## Key Features
- Dynamic prompt showing the current working directory
- Execution of external programs using fork() and exec()
- Built-in commands handled directly by the shell
- Input (<) and output (>) redirection
- Error handling using standard system error reporting
- Demonstrates core systems programming concepts including process management, file descriptors, and OS interaction
- POSIX system calls, specifically to handle signaling for commands like ^C

## Implemented Features
- ^C (Ctrl + C) - Sends an interrupt signal (SIGINT) to the child program.
- cd - navigates directories like expected in a terminal
- history - prints the history of commands, listed in numerical order.

## External Commands
Commands not implemented as built-ins are executed using:
- fork()
- execvp()
- wait()
- dup2()
  
### Examples:
```
ls
ls -l
pwd
cat file.txt
```

## Input and Output Redirection
Supported operators:
- `command > file` — redirect standard output
- `command < file` — redirect standard input

### Examples:
```
ls > output.txt
cat < input.txt
```

## Known Limitation
Currently, the system does not have a built-in line editor, nor a window for the terminal to exist in.
This means that, for one, the system runs within the built-in terminal, creating a not-so-aesthetically pleasing look.
No line editor means that certain actions like the up arrow (which prints out ASCII arrow key codes like `^[[A`) are simply printed out,
instead of doing the tasks that are normally associated with those button presses (in the example of the up arrow, printing out the last typed line).

These two limitations are within the plans to be worked on and fixed, pretty much creating an entirely separate Unix-like terminal. For whatever reason, in case somebody might want a less efficient, external terminal.

## Build Instructions

To clone, do the following steps:

1. Create a new folder in your IDE.
2. Visit the webpage:
    [github.com/EdwardWare06/techshell](https://github.com/EdwardWare06/techshell)
3. Click code, then copy the link provided.
4. Run the following command:
```git
git clone <link> <folder name>
```

To compile and run the file:

1.  Navigate to the project directory:
```bash
    cd <folder name>
```

2. Run the Make file:
```bash
    make
```

3. Run the shell:
```bash
    ./techshell.o
```

## Alternate Compilation

1. Navigate to the project directory like above.

2. Compile the source code:
```bash
    gcc -std=c11 -Wall -Wextra -pedantic -o techshell
```

3. Run the shell:
```bash
    ./techshell
```

4. (Optional) To clean build artifacts:
```bash
    rm -f techshell *.o
```


This project includes a built-in debugging system designed to make development, testing, and troubleshooting easier without affecting normal operation.

Debugging output is controlled entirely at compile time using preprocessor macros. When debugging is disabled, all debug code is completely removed by the compiler and has zero runtime cost.

## Enabling Debug Mode

Debug mode is enabled by defining the `DEBUG` macro at compile time.

### Compile With Debugging Enabled

```bash
gcc -DDEBUG -std=c11 -Wall -Wextra -Werror -pedantic techshell.c -o techshell
```
