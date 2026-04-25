#define _GNU_SOURCE // required for strsignal().
#include <stdlib.h>  // required for EXIT_FAILURE.
#include <errno.h>  // required for errno.
#include <stdio.h>
#include <string.h>  // required for strerror().
#include <unistd.h>
#include <sys/wait.h>  //} these two are for waitpid().
#include <sys/types.h> //} ----------------------------

#include "funcs.h"

int main() {  // not using: "int argc, char **argv", for this project.
    pid_t childPid = -1;
    char* cmdLine;
    char** info;

    // waitpid():
    int stat_loc; // => this variable is used to evaluate the status of the running process.
    int options = 0; // makes waitpid() blocking, until the child-process completes. => 
    // this fits well for a shell-like inviroment.
    //

    printf("---Starting Custom Shell---\n---------------------------\n");

    while (1) {
        errno = 0; // reset for every failed-command's errno value.
        printf("Shelly_v3.0> ");
        fflush(stdout); // making sure it flushes on each iteration, just for safety.

        cmdLine = readline();  // expected intput: main ls-l-a-v-c.
        if (cmdLine == NULL) { // error handling for readline():
            printf("Shell error: %s (errno: %d).\n", strerror(errno), errno);
            break;
        }

        info = parse(cmdLine);
        if (info == NULL) { // error handling for parse():
            printf("Shell error: %s (errno: %d).\n", strerror(errno), errno);
            free(cmdLine);
            continue;
        }

        childPid = fork();
        if (childPid == -1) { // error handling for fork():
            printf("Error forking - %s: %s (errno: %d).\n", info[0], strerror(errno), errno);
            continue;

        } else if (childPid == 0) { // child code:
            executeCommand(info);

        } else { // parent code:
            if (waitpid(childPid, &stat_loc, options) == -1){
                printf("Process error in: %s (pid: %d).\n", info[0], childPid);
                break;
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
            // freeing heap-allocated memory:
            //---
            free(cmdLine);
            free(info);
            //---
        }
    }
}