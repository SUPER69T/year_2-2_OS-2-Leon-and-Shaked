#ifndef FUNCS_H
#define FUNCS_H

#include <errno.h>  // required for errno.
#include <stdio.h>
#include <string.h>  // required for strerror().
#include <sys/types.h>
#include <unistd.h>

/**
 * @operation: takes input as a single chars-array (string). stops at - '\n'.
 *
 * @param[in] None.
 *
 * @returns: char* info: a non-parsed, not-evaluated array of type - 'char*' - 
 * containing both the linux-command, and it's parameters, as well as white-spaces.
**/
char* readline(); 

/**
 * @operation: parsing the array of strings by whitespaces into an array of strings.
 *
 * @param[in] char** info: a linux-command and it's flags / parameters.
 *
 * @returns: a parsed array o strings (of type - 'char**').
**/
char** parse(char* cmdLine); 


/**
 * @operation: executing the parsed strings by using the execlp()-command -
 * forking it's process and returning the pid for error evaluation.
 *
 * @param[in] char** info: a linux-command and it's flags / parameters.
 *
 * @return errno (of int-type).
**/
int executeCommand(char** info);

// // waiting for the current child-fork: 
// waitpid(childPid);

// int pwd()
// cd()
// nano()
// cat()
// wc()
// cp()
// pipe()
// clear()
// grep()
// ls()
// tree(); //(opendir), (dirent.h=>readdir), (closedir), (execvp)

// extern char[] subpath;
// extern int level;
// print_tree(char[] subpath, int level);


#endif // FUNCS_H