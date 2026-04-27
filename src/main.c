#define _GNU_SOURCE // required for strsignal().
#include <stdlib.h>  // required for EXIT_FAILURE.
#include <errno.h>  // required for errno.
#include <stdio.h>
#include <string.h>  // required for strerror().
#include <unistd.h>
#include <sys/wait.h>  //} these two are for waitpid().
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
    // input + process:
    //size_t info_len = 0;
    Function* builtin_func = NULL;
    pid_t childPid = -1;
    char* cmdLine = NULL;
    char** info = NULL;
    //
    
    // for the time printing:
    time_t rawtime;
    struct tm *ctime;
    char buffer[1024] = {0};
    //

    Function pwd = {0, "pwd"};
    Function cd = {1, "cd"};
    Function exit = {2, "exit"};

    Function* funclist = calloc(256, sizeof(Function)); // chose 256 for no special reason.
    if (funclist == NULL) {
        Print2Shelly("Shell error: malloc failed.", 20, 0);
        Print2Shelly("goodbye...", 20, 0);
        return 1;
    }
    
    funclist[0] = pwd;
    funclist[1] = cd;
    funclist[2] = exit;
    
    Fmap functions = {
        .list = funclist,
        .size = 3,  
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
        errno = 0; // reset for every failed-command's errno value.
        fflush(stdout); // making sure it flushes on each iteration, just for safety.

        // printing default line:
        //---
        time(&rawtime); // gets the current calendar time.
        ctime = localtime(&rawtime);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%H:%M:%S", ctime);
        snprintf(buffer, sizeof(time_str),"[%s]> ", time_str);
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

        if (info == NULL) { // error handling for parse():
            char error_msg[256];
            snprintf(error_msg, sizeof(error_msg), "Shell error: %s (errno: %d).\n", strerror(errno), errno);
            Print2Shelly(error_msg, 20, 0);
            free(cmdLine);
            Print2Shelly("Invalid format, please try again...", 20, 0);
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
        for (size_t i = 1; i < functions.size; i++){ 
            if (functions.list[i].value != NULL && strcmp(functions.list[i].value, info[0]) == 0) { // found a builtin function:
                builtin_func = &functions.list[i];
                switch (builtin_func->key) {
                    case 0: {// pwd.
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
                    case 1: {// cd.
                        if (info[1] == NULL) { // no path argument passed to cd:
                            char error_msg[256];
                            snprintf(error_msg, sizeof(error_msg), "Command error: %s did not receive a path argument.\n", info[0]);
                            Print2Shelly(error_msg, 20, 0);
                            break;
                        }
                        chdir(info[1]);
                        break;
                    }
                    case 2: {// exit.
                        Print2Shelly("goodbye...", 20, 0);
                        cleanup(cmdLine, info, &functions);
                        return 0;
                    }
                }
                // manual cleanup on normal child return:
                //---
                free(cmdLine);
                free(info);
                // additional safety against double-freeing of memory:
                cmdLine = NULL;
                info = NULL;
                //---
                continue;
            }
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