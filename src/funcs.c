#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "funcs.h"

char* readline() { // RE-READ THIS CODE AGAIN.
    int size = 1024;
    int len = 0;
    char* str = malloc(size * sizeof(char));
    int c; // getchar() returns an unsigned char.

    if (!str) return NULL;

    while ((c = getchar()) != '\n' && c != EOF) {
        str[len++] = (char)c; // casting c from int to a char.

        if (len >= size) { // re-allocation of the str memory-space on the heap.
            size *= 2;
            char* temp = realloc(str, size * sizeof(char));
            if (!temp) {
                free(str); // freeing the original string if realloc fails.
                return NULL;
            }
            str = temp;
        }
    }
    str[len] = '\0'; // null-terminate the string.
    return str;
}


char** parse(char* cmdLine) {
    // cmdLine = {'abc d efgh i'}.
    // parsed_strs = {{'abc'}, {'d'}, {'efgh'}, {'i'}}.
    char** parsed_strs[1024];
    char current_char;
    size_t current_word_index = 0;
    size_t word_index = 0;

    size_t len = sizeof(cmdLine) / sizeof(cmdLine[0]);
    for (size_t i = 0; i < len; i++) { 
        current_char = cmdLine[i];
        if (current_char == ' ') { 
            current_word_index++; // moving to the next word.
            // initializing the current word's index to - 0:
            word_index = 0;
            continue;
        }
        parsed_strs[current_word_index][word_index] = &current_char;
        word_index++; // moving to the next char in the current word.
    } 
    
    // cleaning the output strings-array:
    //---
    for (size_t j = 0; j < current_word_index; j++) {

    }
    //---



    // int size = 64;
    // int len = 0;
    // char** tokens = malloc(size * sizeof(char*));
    // char* token;

    // if (!tokens) return NULL;

    // token = strtok(cmdLine, " \t\r\n"); // split by spaces, tabs, carriage returns, and newlines.
    // while (token != NULL) {
    //     tokens[len++] = token;

    //     if (len >= size) {
    //         size *= 2;
    //         char** temp = realloc(tokens, size * sizeof(char*));
    //         if (!temp) {
    //             free(tokens); // freeing the original tokens array if realloc fails.
    //             return NULL;
    //         }
    //         tokens = temp;
    //     }
    //     token = strtok(NULL, " \t\r\n");
    // }
    // tokens[len] = NULL; // null-terminate the array of tokens.
    // return tokens;


}



int pwd(int token) {

    execlp("pwd", "pwd", (char *)NULL); // gemini: "Casting NULL:" =>
    //"In many environments, it is safer to cast the sentinel value: (char *)NULL"...
    return errno; // only gets here on failure. 
}


// int main() {
//     ssize_t pid;
//     int myPipeFD[2];
//     int ret;
//     char buf[40];

//     // Create pipe
//     ret = pipe(myPipeFD);
//     if (ret == -1) {
//         perror("pipe error");
//         exit(1);
//     }

//     // Create child process
//     pid = fork();
//     if (pid == 0) {
//         // Child process
//         printf("bullshit1 process\n");
//         write(myPipeFD[1], "hello , the are someone?!\n", 30);
//     } 
//     else {
//         // Parent process
//         printf("bullshit0 process\n");
//         read(myPipeFD[0], buf, 30);
//         printf("i am read message from buffer: %s\n", buf);
//     }

//     return 0;
// }