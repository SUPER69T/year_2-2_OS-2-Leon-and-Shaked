#ifndef FUNCS_H
#define FUNCS_H

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>

// building a map of available custom-functions:
//---
    typedef struct {
    int key;
    char* value;
} Function;

typedef struct {
        Function* list;
        size_t size;
        size_t capacity;
} Fmap;
// this way we allow easier expansion for a list of functions into our shell.
//---

void Print2Shelly(char* word, int interval_ms, int no_newline);

/**
 * @operation: takes stdin's input as a single chars-array (string). stops at - '\n'.
 *
 * @param[in] None.
 *
 * @returns: char* info: a non-parsed, not-evaluated array of type - 'char*' - 
 * containing both the linux-command, and it's parameters, as well as: ('\t','\r','\0',' ').
**/
char* readline(); 

/**
 * @operation: tokenizing the array of strings into an array of tokens (first char's pointers).
 *
 * @param[in] char** info: a linux-command and it's flags / parameters.
 *
 * @returns: a tokenized array of strings (of type - 'char**').
**/
char** parse(char* cmdLine); 

/**
 * @operation: executing the parsed strings by using the execv()-command -
 * replacing the child process with a new running process of the desired command.
 *
 * @param[in] char** info: a linux-command and it's flags / parameters.
 *
 * @return: void.
**/
void executeCommand(char** info);

void tree();

void print_tree(char* subpath, int level);

#endif // FUNCS_H