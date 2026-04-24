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
        errno = 0; // reset for every command error-return check.
        printf("---Starting Custom Shell---\n---------------------------");
        cmdLine = readline();  // expected intput: main ls-l-a-v-c.
        info = parse(cmdLine);
        childPid = fork();
        if (childPid == 0) {
            /*------------------------------- child code */
            errno = executeCommand(info);
            if (errno != 0) {
                printf("Error running - %s: %s (errno: %d)\n", info[0], strerror(errno), errno);
                continue;
            }
        } else {
            /*------------------------------- parent code */
            //waitpid(childPid);
        }
    }
}