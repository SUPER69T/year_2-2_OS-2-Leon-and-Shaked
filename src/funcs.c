#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "funcs.h"

char* readline() { 
    int size = 1024;
    int len = 0;
    char* str = malloc(size * sizeof(char)); // =>
    // here malloc() allocates a modifiable character array on the heap. =>
    // this is going to be useful for parse() in order to tokanize 'cmdLine'.

    if (!str) return NULL; // returns on error.    
    int c; 
    while ((c = getchar()) != '\n' && c != EOF) { // =>
        // getchar() returns an ascii code representing the next char in input.

        str[len++] = (char)c; // casting c from int to a char. =>
        // incrementing len only after assigning str[len] = (char)c.

        if (len >= size) { // re-allocation of the str memory-space on the heap.
            size *= 2;
            char* temp = realloc(str, size * sizeof(char));
            if (!temp) {
                free(str); // freeing the original string if realloc fails.
                return NULL; // returns on error.
            }
            str = temp;
        }
    }
    str[len] = '\0'; // null-terminate the string.
    return str;
}


char** parse(char* cmdLine) {
    // example of operation:
    // cmdLine = {*'abc d efgh i'}.
    // parse(cmdline) = {*'abc', *'d', *'efgh', *'i'}.

    size_t max_tokens = 4; // |command(1) + flags| = 4 (as a default reference..).
    char** tokens = malloc(max_tokens * sizeof(char*)); 

    if (!tokens) return NULL; // returns on error.

    char* token = cmdLine; // manually assigning the first token to be the entire word.
    
    size_t current_token_count = 0; // will also be used as the overall-tokens-count.
    
    size_t len = strlen(cmdLine);
    char* current_char;

    int previous_was_char = 1; // used as a boolean indicator of a previous instance of char to battle the case in which -
    // we have multiple deliminating symbols in a row.

    for (size_t i = 0; i < len; i++) { // manually tokanizing cmdLine:
        current_char = &cmdLine[i];
        if ((*current_char == '\t') || (*current_char == '\r') || (*current_char == '\n')) {

            *current_char = '\0'; // =>
            // swapping the-('\t','\r','\n') unsigned-chars with-'\0' for standardized parsing convenience in-C/C++.

            if (previous_was_char != 1) continue;
            previous_was_char = 0;

            // resizing 'tokens''s heap memory if needed:
            //---
            if (current_token_count >= max_tokens - 1) {
                max_tokens *= 2;
                char** temp = realloc(tokens, max_tokens * sizeof(char*));
                if (!temp) { // freeing the original tokens array if realloc fails and returning 'NULL' to indicate an error:
                    free(tokens); 
                    return NULL; 
                }
                tokens = temp;
            }
            //---


            tokens[current_token_count] = token; // this is a necessary re-assignment of the first pointer because of -
            // the special case in which we never do find additional tokens (in the case of a parameterless command..).
        
            token = (char*)malloc(sizeof(char*)); // =>
            // allocating memory on the heap, pointing to the next token's first char.

            if (!token) return NULL; // returns on error.
            
            *token = *current_char; // =>
            // assigning current_char's data to the heap-allocated address on the heap.
            
            current_token_count++;
        } else {
            if (previous_was_char == 0) {
                tokens[current_token_count] = token; // this is a necessary re-assignment of the first pointer because of -
                // the special case in which we never do find additional tokens (in the case of a parameterless command..).
                
                *token = *current_char; // =>
                // assigning current_char's data to the heap-allocated address on the heap.
                current_token_count++;
            }
            previous_was_char = 1;
        }
    }
    return tokens;
}

    // int size = 64;
    // int len = 0;
    // char** tokens = malloc(size * sizeof(*tokens)); // sizeof(*tokens) = sizeof(char*).
    // char* token;

    // if (!tokens) return NULL; 

    // token = strtok(cmdLine, " \t\r\n"); // split by spaces, tabs, carriage returns, and newlines.
    // while (token != NULL) {
    //     tokens[len++] = token;

    //     if (len >= size - 1) {
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


//}



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