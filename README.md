# TechShell

## Group Members
Edward Ware
Cole Pellegrin
Carter Landry

## Description
TechShell is a Unix-like shell written in C (specifically the C11 standard) using POSIX system calls.
It supports command execution, directory navigation, I/O redirection,
pipes, background execution, and detailed error handling.


## Build Instructions

In order to clone, do these following steps:

1. Create a new folder in your IDE.
2. Visit the webpage:
    [github.com/EdwardWare06/techshell](https://github.com/EdwardWare06/techshell)
3. Click code, then copy the link provided.
4. run the following command:
```git
git clone <link> <folder name>
```

In order to compile and run the file:

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
