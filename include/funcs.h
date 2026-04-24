#ifndef FUNCS_H
#define FUNCS_H

#include <errno.h>  // required for errno.
#include <stdio.h>
#include <string.h>  // required for strerror().
#include <sys/types.h>
#include <unistd.h>

// taking input as a single chars-array (string). stops at - '\n':
char* readline(); 

// parsing the array of strings by whitespace into tokens chars-arrays (strings):
char** parse(char* cmdLine); 


/**
 * executing the parsed strings by using the execlp()-command -
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