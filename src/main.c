#define _GNU_SOURCE // required for strsignal().
#include <stdlib.h>  // required for EXIT_FAILURE.
#include <errno.h>  // required for errno.
#include <stdio.h>
#include <string.h>  // required for strerror().
#include <unistd.h>
#include <sys/wait.h>  //} these two are for waitpid().
#include <unistd.h> // required for sleep().
#include <time.h> // required for 'tm'-struct.

#include "funcs.h"

void Print2Shelly(char* word, int interval_ms) { 
    interval_ms *= 1000;
    int period_interval = interval_ms * 5;
    int word_len = strlen(word);
    for (int i = 0; i < word_len; i++) {
        usleep(interval_ms);
        if (word[i] == ',' || word[i] == '.') {
            usleep(period_interval);
        }
        printf("%c", word[i]);
    }
    printf("\n");
}

void cleanup(char* cmdLine, char** info, size_t info_len,Fmap* functions) {
    free(cmdLine);

    if (info != NULL) {
        for (size_t i = 0; i < info_len; i++){
            free(info[i]);
        }
        free(info);
    }

    if (functions != NULL) {
        free(functions->list);
        free(functions);
    }
}


int main() {  // not using: "int argc, char **argv", for this project.
    // input + process:
    size_t info_len = 0;
    Function* builtin_func;
    pid_t childPid = -1;
    char* cmdLine;
    char** info;
    //
    
    // for the time printing:
    time_t rawtime;
    struct tm *ctime;
    char buffer[80];
    buffer[0] = '[';
    //

    Function pwd = {1, "pwd"};
    Function cd = {2, "cd"};
    Function exit = {3, "exit"};

    Function* funclist = malloc(sizeof(Function) * 256); // chose 256 for no special reason.
    
    funclist[1] = pwd;
    funclist[2] = cd;
    funclist[3] = exit;
    
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

    Print2Shelly("<--Starting Shelly_v1.0-->\n|------------------------|\n", 150);

    while (1) {
        errno = 0; // reset for every failed-command's errno value.
        fflush(stdout); // making sure it flushes on each iteration, just for safety.

        // printing default line:
        //---
        time(&rawtime); // gets the current calendar time.
        ctime = localtime(&rawtime);
        strftime(&buffer[1], sizeof(buffer) - 1, "%H:%M:%S", ctime); // =>
        // "&buffer[1])": adding the formatted time from the second position of the buffer.
        strcat(buffer, "]> ");
        Print2Shelly(buffer, 200);
        //---

        cmdLine = readline();  // expected intput: main ls-l-a-v-c.
        if (cmdLine == NULL) { // error handling for readline():
            printf("Shell error: %s (errno: %d).\n", strerror(errno), errno);
            Print2Shelly("goodbye...", 200);
            cleanup(cmdLine, info, info_len, &functions);
            return 1;
        }

        info = parse(cmdLine);

        if (info == NULL) { // error handling for parse():
            printf("Shell error: %s (errno: %d).\n", strerror(errno), errno);
            free(cmdLine);
            Print2Shelly("Invalid format, please try again...", 200);
            //cleanup(cmdLine, info, info_len, &functions);
            continue;
        }

        // calculating the size of 'info' for the deallocation. could be optimized if we returned the len from - parse()...
        for (size_t  i = 0; info[i] != NULL; i++) {
            info_len++;
        }
        //

        // custom function calls:
        //---
        // found this to be the easiest implementation of builtin functions / directory-dependant functions...
        for (size_t i = 0; i < functions.size; i++){ 
            if (strcmp(functions.list[i].value, info[0]) == 0) { // found a builtin function:
                builtin_func = &functions.list[i];
                switch (builtin_func->key) {
                    case 1: // pwd.
                        char cwd[1024];
                        if (getcwd(cwd, sizeof(cwd)) != NULL) {
                            printf("%s\n", cwd);
                        } else {
                            printf("Command error: %s couldn't execute.\n", info[0]);
                        }
                        break;
                    case 2: // cd.
                        if (info[1] == NULL) { // no path argument passed to cd:
                            printf("Command error: %s did not receive a path argument.\n", info[0]);
                            Print2Shelly("Invalid format, cd requires a - '$PATH'...", 200);
                            break;
                        }
                        chdir(info[1]);
                        break;
                    case 3: // exit.
                        Print2Shelly("goodbye...", 200);
                        cleanup(cmdLine, info, info_len, &functions);
                        return 0;
                }
                free(cmdLine);
                free(info);
                continue;
            }
        }
        //--- 
        //


        childPid = fork();
        if (childPid == -1) { // error handling for fork():
            printf("Error forking - %s: %s (errno: %d).\n", info[0], strerror(errno), errno);
            // cleanup(cmdLine, info, info_len, &functions);
            continue;

        } else if (childPid == 0) { // child code:
            executeCommand(info);
            _exit(EXIT_FAILURE);
            
        } else { // parent code:
            if (waitpid(childPid, &stat_loc, options) == -1){
                printf("Process error in: %s (pid: %d).\n", info[0], childPid);
                Print2Shelly("goodbye...", 200);
                cleanup(cmdLine, info, info_len, &functions);
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
                    printf("Command failed with exit code: %d....\n", exit_code);
                }
            }
            else if (WIFSIGNALED(stat_loc)) { // WTERMSIG should only be called if WIFSIGNALED is true.
                int sig_num = WTERMSIG(stat_loc);
                printf("Error running - %s: Killed by signal %d (%s).\n", 
                info[0], sig_num, strsignal(sig_num)); // =>
                // strsignal() is used to get a description of a signal.
            }
            // manual cleanup on normal child return:
            free(cmdLine);
            free(info);
            //
        }
    }
}