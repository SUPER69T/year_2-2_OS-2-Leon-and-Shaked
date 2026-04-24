#include <errno.h>  // required for errno.
#include <stdio.h>
#include <string.h>  // required for strerror().
#include <sys/types.h>
#include <unistd.h>

#include "funcs.h"

int main() {  // not using: "int argc, char **argv", for this project.
    pid_t childPid = -1;
    char* cmdLine;
    char** info;

    while (1) {
        errno = 0; // reset for every failed-command's errno value.
        printf("---Starting Custom Shell---\n---------------------------");
        cmdLine = readline();  // expected intput: main ls-l-a-v-c.
        if (cmdLine == NULL) { // error handling for readline():
        printf("Shell error: %s (errno: %d).\n", strerror(errno), errno);
                break;
        }
        info = parse(cmdLine);
        if (info == NULL) { // error handling for parse():
        printf("Shell error: %s (errno: %d).\n", strerror(errno), errno);
                break;
        }
        childPid = fork();
        if (childPid == -1) { // error handling for fork():
                printf("Error forking - %s: %s (errno: %d).\n", info[0], strerror(errno), errno);
                continue;
        }
        if (childPid == 0) {
            /*------------------------------- child code */
            errno = executeCommand(info);
            if (errno != 0) {
                printf("Error running - %s: %s (errno: %d).\n", info[0], strerror(errno), errno);
                continue;
            }
        } else {
            /*------------------------------- parent code */
            //waitpid(childPid);
        }
    }
}