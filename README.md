# TechShell

## Author
Edward Ware

## Description
TechShell is a Unix-like shell written in C (specifically the C13 standard) using POSIX system calls.
It supports command execution, directory navigation, I/O redirection,
pipes, background execution, and detailed error handling.

## Build Instructions

1. Navigate to the project directory:
    cd "Bash Shell Project"

2. Compile the source code:
    gcc -std=c11 -Wall -Wextra -o techshell

3. Run the shell:
    ./techshell

4. (Optional) To clean build artifacts:
    rm -f techshell *.o
