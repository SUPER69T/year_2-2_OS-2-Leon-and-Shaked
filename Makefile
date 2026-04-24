# when starting a new project:
# change both the 'main'-target (l-53) and the -
# 'main'-output-filename, to your desired executable filename (l-54).


# make-command's colon-syntax:
# target: dependencies.
#
# Target = What you want to create (the file).
# Dependencies = What is needed to make that file.

# parameterless-'make' is choosing targets lazily, the first target -
# it detects is the one that's getting executed. 
# for additional target execution, the name of the target is required:
# 'make clean', 'make link', 'make whatever'...


#---
# For safety incase of file-naming conflicts:
.PHONY: all clean
# This tells the make-command that the name isn't a file.
#---


#VARIABLES-DEC for EVAL:
#---
CC = clang
FLAGS = -std=c11 -Wall -Wextra -Iinclude 
CFLAGS = $(FLAGS) -g
#
# This automatically finds all '.c' files:
SRCS = $(wildcard src/*.c) # => src/main.c src/funcs.c...
# 'VPATH = src:include' is a comfortable solution for dynamic lookup.
#
# This (inside "main: $(OBJS)") calls the magic BS in lines(62-65) -
# in order to evaluate and get files with the '.o' postfixes, causing -
# the additional '.c' to '.o' compilations when required:
OBJS = $(SRCS:.c=.o) # => src/main.o src/funcs.o...
# List of all '.h' files:
DEPS = $(wildcard include/*.h)
#---


#---
all: main compile_flags.txt # (make-command's entry point in this file)
# all => triggers both 'main' (linking/compiling), and LSP (flags updating).
#---


#-----
#---
# LINKING: 
main: $(OBJS) 
	$(CC) $(OBJS) -o main
# => the line above prints into the terminal so the linking -
# process will take place, but only if 
#---


#---
# This turns every (updated) .c  file into an .o (object) file:
%.o: %.c $(DEPS) # this line compares the timestamps of both file-types -
# with the object-file type, triggering the next line to print into the -
# terminal, causing the compiler to (re/)compile the '.c' file into an '.o' file:
	$(CC) $(CFLAGS) -c $< -o $@  
# => the line above expands into: 
#clang -std=c11 -Wall -Wextra -I. -Iinclude -g -c 'filename'.c  -o 'filename.o'
# => the output is: 'filename.o'.
#
# %: Make's wildcard for removing postfixes after the dot.
# $<: An automatic variable meaning "The first dependency" (the .c file).
# $@: An automatic variable meaning "The target" (the .o file).
#
# '-c' flag: Compiles source code into object files (.o) without linking.
#---
#-----


#---
# This automatically updates - 'compile_flags.txt' for better LSP-linting:
compile_flags.txt:
	echo "$(FLAGS)" | tr ' ' '\n' > compile_flags.txt 
# " tr ' ' '\n' ": bash's translate command, swapping each of the -
# ' ' spaces in between the compiler-flags in $FLAGS with '\n'.
#---


#---
# This deletes - 'main', 'compile_flags.txt', and every '.o' file:
clean:
	rm -f main compile_flags.txt $(OBJS) 
# Triggers on command: 'make clean' in the terminal.
#---