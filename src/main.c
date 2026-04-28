#define _GNU_SOURCE // required for strsignal().
#define READ 0  //} these two are required for piping the stdout -
#define WRITE 1 //} tunnel from the forked child-process to Print2Shelly().

#include <stdlib.h>  // required for EXIT_FAILURE.
#include <errno.h>  // required for errno.
#include <stdio.h>
#include <string.h>  // required for strerror().
#include <unistd.h>
#include <sys/wait.h>  // requiured for waitpid().
#include <time.h> // required for 'tm'-struct.

#include "funcs.h"


void cleanup(char* cmdLine, char** info, Fmap* functions) {
    if (cmdLine != NULL) {
        free(cmdLine);  // cmdLine is the only heap-allocated memory from readline().
    }

    if (info != NULL) {
        free(info); // => info array itself was allocated by parse(), 
        // but tokens point into cmdLine so we only free the array, not individual elements.
    }

    if (functions != NULL && functions->list != NULL) {
        free(functions->list);  // freeing the heap-allocated array, not the struct itself.
    }
}


int main() {  // not using: "int argc, char **argv", for this project.

    // using of builtin functions:
    Function* builtin_func = NULL;
    int found_builtin;
    // 
    
    // input:
    //size_t info_len = 0;
    char* cmdLine = NULL;
    char** info = NULL;
    //

    // forking:
    pid_t childPid = -1;
    //

    // for time printing:
    time_t rawtime;
    struct tm *ctime;
    char buffer[1024] = {0};
    //

    Function pwd = {0, "pwd"};
    Function cd = {1, "cd"};
    Function exit = {2, "exit"};
    Function ls = {3, "ls"};

    Function* funclist = calloc(256, sizeof(Function)); // chose 256 for no special reason.
    if (funclist == NULL) {
        Print2Shelly("Shell error: malloc failed.", 20, 0);
        Print2Shelly("goodbye...", 20, 0);
        return 1;
    }
    
    funclist[0] = pwd;
    funclist[1] = cd;
    funclist[2] = exit;
    funclist[3] = ls;
    
    Fmap functions = {
        .list = funclist,
        .size = 4,  
        .capacity = 256 
    };
    //---


    // waitpid():
    int stat_loc; // => this variable is used to evaluate the status of the running process.
    int options = 0; // makes waitpid() blocking, until the child-process completes. => 
    // this fits well for a shell-like inviroment.
    //

    Print2Shelly("<--Starting Shelly_v1.0-->\n|------------------------|\n", 15, 0);

    while (1) {
        // resets:
        errno = 0; // reset for every failed-command's errno value.
        found_builtin = 0;
        fflush(stdout); // making sure it flushes on each iteration, just for safety.
        //

        // printing default line:
        //---
        time(&rawtime); // gets the current calendar time.
        ctime = localtime(&rawtime);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", ctime);
        snprintf(buffer, sizeof(buffer),"[%s]> ", time_str);
        Print2Shelly(buffer, 20, 1);
        //---

        cmdLine = readline();  // expected intput: main ls-l-a-v-c.
        if (cmdLine == NULL) { // error handling for readline():
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Shell error: %s (errno: %d).", strerror(errno), errno);
            Print2Shelly(error_msg, 20, 0);
            Print2Shelly("goodbye...", 20, 0);
            cleanup(cmdLine, info, &functions);
            return 1;
        }

        info = parse(cmdLine);

        if (info == NULL || info[0] == NULL) { // error handling for parse():
            if (info) free(info);
            free(cmdLine);
            Print2Shelly("You're wasting CPU cycles for nothing...", 20, 0);
            //cleanup(cmdLine, info, info_len, &functions);
            continue;
        }

        // calculating the size of 'info' for the deallocation. could be optimized if we returned the len from - parse()...
        // for (size_t  i = 0; info[i] != NULL; i++) {
        //     info_len++;
        // }
        //

        // custom function calls:
        //---

        // found this to be the easiest implementation of builtin functions / directory-dependant functions:
        for (size_t i = 0; i < functions.size; i++){ 
            if (functions.list[i].value != NULL && strcmp(functions.list[i].value, info[0]) == 0) { // found a builtin function:
                builtin_func = &functions.list[i];
                found_builtin = 1;
                switch (builtin_func->key) {
                    case 0: { // pwd.
                        char cwd[1024];
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            Print2Shelly(cwd, 20, 0);
                        } else {
                            char error_msg[256];
                            snprintf(error_msg, sizeof(error_msg), "Command error: %s couldn't execute.\n", info[0]);
                            Print2Shelly(error_msg, 20, 0);
                        }
                        break;
                    }
                    case 1: { // cd.
                        if (info[1] == NULL) { // no path argument passed to cd:
                            char error_msg[256];
                            snprintf(error_msg, sizeof(error_msg), "Command error: %s did not receive a path argument.\n", info[0]);
                            Print2Shelly(error_msg, 20, 0);
                            break;
                        }
                        DIR* dir = opendir(info[1]);
                        if (dir == NULL) { // not a directory or can't open.
                            char error_msg[256];
                            snprintf(error_msg, sizeof(error_msg), "Command error: %s improper path argument.\n", info[0]);
                            Print2Shelly(error_msg, 20, 0);
                            break;
                        }
                        chdir(info[1]);
                        closedir(dir);
                        break;
                    }
                    case 2: { // exit.
                        Print2Shelly("goodbye...", 20, 0);
                        cleanup(cmdLine, info, &functions);
                        return 0;
                    }
                    case 3: { // ls.
                        // creating a pipe:
                        int fds[2];
                        if (pipe(fds) == -1) {
                            Print2Shelly("Error: pipe failure...", 20, 0);
                            break;
                        }
                        //
                        
                        childPid = fork();
                        if (childPid == -1) { // error handling for fork():
                            char error_msg[256];
                            snprintf(error_msg, sizeof(error_msg), "Error forking - %s: %s (errno: %d).\n", info[0], strerror(errno), errno);
                            Print2Shelly(error_msg, 20, 0);
                            close(fds[0]);
                            close(fds[1]);
                        } 
                        else if (childPid == 0) { // child process:
                            dup2(fds[WRITE], STDOUT_FILENO); // =>
                            // dup2() is redirecting standard output to the pipe's write-end.
                            // dupe2() uses "block-buffering"...

                            // closing both pipe-ends:
                            close(fds[READ]); 
                            close(fds[WRITE]);
                            //

                            execlp("/bin/ls", "ls", NULL);
                            _exit(EXIT_FAILURE);
                        } 
                        else { // parent process:
                            close(fds[WRITE]); // =>
                            // closing write-end in the parent, before the parent tries to read (preventing an infinite reading loop).

                            char ls_buffer[4096]; // the buffer will hold stdout's output.
                            ssize_t count;

                            // reading until the child-process closes it's pipe-end:
                            while ((count = read(fds[READ], ls_buffer, sizeof(ls_buffer) - 1)) > 0) { // =>
                                // count is the actual number of character the child process sent through the pipe.
                                ls_buffer[count] = '\0';
                                Print2Shelly(ls_buffer, 20, 0); 
                            }
                            close(fds[READ]); // closing read-end.
                            wait(NULL); // waiting for the child-process to exit.
                        }
                        break;
                    }
                }
                break; // break out of the for loop when a builtin is found
            }
        }
        
        // handlןמע builtin cleanup and skipping fork() for builtin commands:
        if (found_builtin) {
            // manual cleanup on normal child return:
            //---
            free(cmdLine);
            free(info);
            // additional safety against double-freeing of memory:
            cmdLine = NULL;
            info = NULL;
            //
            continue; // going to the next prompt.
        }
        //--- 
        //


        childPid = fork();
        if (childPid == -1) { // error handling for fork():
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Error forking - %s: %s (errno: %d).\n", info[0], strerror(errno), errno);
            Print2Shelly(error_msg, 20, 0);
            // cleanup(cmdLine, info, info_len, &functions);
            continue;

        } else if (childPid == 0) { // child code:
// DEPRICATED:
//---
// dup2(fds[1], STDOUT_FILENO); // =>
// // dup2() is redirecting standard output to the pipe's write-end.
// dup2(fds[1], STDERR_FILENO);

// // closing descriptors that are no longer needed:
// close(fds[READ]); 
// close(fds[WRITE]);
//--- 
// this was a cute idea to pipe all child-process std-out to Print2Shelly, but that would -
// have introduced a new giant problem, which is restricting commands that require the use -
// of the std-in/out in order to function normally (echo, man, less...).
// this idea is definitely possible by adding commands to the builtins struct and manually -
// deciding how they work/print/function by overriding and modifying their natural workflow -
// , but would require much more time than given for this assignment because that would be like -
// building an actual fully-functional shell...

            executeCommand(info);
            _exit(EXIT_FAILURE);
            
        } else { // parent code:
            if (waitpid(childPid, &stat_loc, options) == -1){
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "Process error in: %s (pid: %d).\n", info[0], childPid);
                Print2Shelly(error_msg, 20, 0);
                Print2Shelly("goodbye...", 20, 0);
                cleanup(cmdLine, info, &functions);
                return 1;
            } // =>
            // MACROS for checking stat_loc's exit status:
            // WIFEXITED: returns true if the process finished normally.
            // WEXITSTATUS: gives the exit code (exit(0) or return 0).
            // WIFSIGNALED: returns true if the process was terminated by a signal.
            // WTERMSIG: gives the signal number (^C (SIGINT) or Segfaul(SIGSEGV)).

            if (WIFEXITED(stat_loc)) { // WEXITSTATUS should only be called if WIFEXITED is true.
                int exit_code = WEXITSTATUS(stat_loc);
                if (exit_code != 0) {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), "Command failed with exit code: %d...\n", exit_code);
                    Print2Shelly(error_msg, 20, 0);
                }
            }
            else if (WIFSIGNALED(stat_loc)) { // WTERMSIG should only be called if WIFSIGNALED is true.
                int sig_num = WTERMSIG(stat_loc);
                char error_msg[256];
                snprintf(error_msg, sizeof(error_msg), "Error running - %s: Killed by signal %d (%s).\n", 
                info[0], sig_num, strsignal(sig_num)); // =>
                // strsignal() is used to get a description of a signal.
                Print2Shelly(error_msg, 20, 0);
            }
            free(cmdLine);
            free(info);
            cmdLine = NULL;
            info = NULL;
        }
    }
}