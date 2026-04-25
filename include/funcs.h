#ifndef FUNCS_H
#define FUNCS_H

#include <sys/types.h>
#include <unistd.h>

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

// tree(); //(opendir), (dirent.h=>readdir), (closedir), (execvp)

// extern char[] subpath;
// extern int level;
// print_tree(char[] subpath, int level);


#endif // FUNCS_H