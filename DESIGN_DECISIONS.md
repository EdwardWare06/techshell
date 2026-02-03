# Design Decisions — TechShell

## Overview

Techshell is a simple Unix-like shell written in C using POSIX system calls.  
This document explains *why* certain design choices were made.
This document also keeps track of newer information for me to reference so I don't have to memorize everything just yet.

The main goals of this project were:
- accuracy
- clarity
- clean compilation under strict compiler flags
- test how far I can push my research and coding skills

Most of the research for this project was done using geeksforgeeks.com and youtube, as well as some stackoverflow and just general googling.

The Google AI response that shows up when you search something was also used for research, but no other generative AI was used in this project.

The program is compiled using:

```bash
-std=c11 -Wall -Wextra -Werror -pedantic
```
This shell relies heavily on POSIX system calls such as fork, execvp, pipe, and sigaction.
When compiling in strict ISO C mode with -pedantic, these functions are not guaranteed to be visible unless POSIX features are explicitly enabled.

To solve this, the following macro is defined before any includes:
```c
#define _POSIX_C_SOURCE 200809L
```
This tells the compiler and standard library headers to expose POSIX.1-2008 functionality.

## Process Creation and CMD Execution
Commands in the shell are executed using the classic Unix pattern:

1. fork() creates a child process
2. execvp() replaces the child with the requested program

execvp() was chosen specifically because:
1. it searches the PATH automatically
2. it works naturally with argument arrays (argv)
3. it avoids hardcoding executable paths
4. I could not use system() as per the assignment details.

## Built-in Commands (cd, exit, history)

Some commands cannot be executed in a child process.

For example, cd must change the shell’s own working directory.
If cd were executed in a child process, the directory change would disappear as soon as the child exits.

Because of this:

1. built-in commands are detected early
2. they are handled directly in the main shell loop
3. no fork() is used for them

For the case of cd, it is detected as a built in command, and the directory change is performed using chdir(), and errors are reported using errno and strerror().

## String Handling
How Input Strings Are Handled

Input is read using fgets() into a fixed-size buffer. This ensures:

* bounded input
* no buffer overflows
* predictable memory usage

After reading input:

1. the trailing newline is removed
2. empty input is ignored
3. the line is tokenized using strtok()

Tokenization splits the input string on spaces and produces an argument vector (argv) that is then used in the execvp() function.

The tradeoff is that strtok() does not support quoting or escaping. 
However, I am too dumb and tired of this project to find out how to do this in a manner that doesn't break everything.


## Output Handling
Normal output is handled automatically by executed programs through standard output.

The shell itself only prints:

* the prompt
* error messages
* built-in command output (such as history)

Error messages are printed using fprintf(stderr, ...), while normal shell output uses stdout.

## Input and Output Redirection (< and >)
Redirection is handled inside the child process, just before executing a command.

The general steps are:

1. detect < or >
2. open the specified file using open()
3. redirect standard input or output using dup2()
4. execute the command with execvp()

This ensures that:

* redirection only affects the command being run
* the shell itself is never accidentally redirected
* redirection works correctly with pipelines

## Background Execution (&)
Background execution is handled by detecting the & symbol during parsing.

If a command is marked as a background job:

* the parent shell does not call wait()
* the shell immediately returns to the prompt

This allows commands like:
```bash
sleep 5 &
```
I refused to try to work with foreground and background operations, because frankly I haven't really ever used fg or bg.

## Pipes (|)
Pipelines are implemented using an iterative approach:

1. the command line is scanned for |
2. each command between pipes becomes a separate process
3. pipe() is used to connect stdout of one process to stdin of the next
4. dup2() performs the actual redirection

The main reason I wanted this is because it supports an arbitrary length of pipelines, such as:
```bash
ls -l | grep "^d" | wc -l
```
Each stage of the pipeline runs in its own process, which was pretty difficult to figure out.

## Signal Handling (Ctrl+C / SIGINT)
Why I used sigaction() instead of signal()

* It was highly recommended on youtube to use that instead when dealing with POSIX handling.
* signal() has implementation-defined behavior and can lead to subtle bugs.

The shell is designed so that:

* the shell itself ignores Ctrl+C
* foreground child processes receive Ctrl+C normally
* the shell remains running and redraws the prompt

To accomplish this:

* the signal handler only sets a sig_atomic_t flag
* blocking calls like fgets() are safely interrupted
* no printing is done inside the signal handler

I kept running into issues when printing inside signal handlers so I just ommitted that. 

## Command History
Command history is implemented as a fixed-size in-memory buffer.

This approach was chosen because:

* it is simple and deterministic
* it avoids dynamic memory management because I knew I would probably end up with a segfault error.
* it is easy to explain

The history built-in prints previously entered commands in order.

## Error Handling
All errors are handled using errno and strerror().

This was just because the assignment recommended it and follows POSIX conventions.

Example output:
```
Error 2 (No such file or directory)
```

## Compilation
All warnings are treated as errors using:

- -Wall
- -Wextra
- -Werror
- -pedantic

-Werror makes all the warnings produced from the other flags produce errors instead.

-Pedantic makes sure I stick to the c11 standard. Though of course it isn't as strict as c99, I much prefer it. 

The goal was to ensure the program is not only functional, but also robust and standards-compliant.

