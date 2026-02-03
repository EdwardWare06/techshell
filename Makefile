CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -pedantic
TARGET = techshell.o

# Build all targets (default target)
all: $(TARGET)

# Link the executable from the source file
$(TARGET): techshell.c
	$(CC) $(CFLAGS) techshell.c -o $(TARGET)

# Remove the compiled executable
clean:
	rm -f $(TARGET)
