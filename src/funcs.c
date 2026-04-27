#include "funcs.h"

#include <errno.h>  // required for errno.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // required for strerror().
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>   //} for tree() / print_tree().
#include <sys/stat.h> //}

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

        // resizing 'str''s heap memory if needed:
        //---
        if (len >= size) { // re-allocation of the str memory-space on the heap.
            size *= 2;
            char* temp = realloc(str, size * sizeof(char));
            if (!temp) {
                free(str); // freeing the original string if realloc fails.
                return NULL; // returns on error.
            }
            str = temp;
        //---
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
    
    size_t current_token_count = 0; // will also be used as the overall-tokens-count.
    
    size_t len = strlen(cmdLine) + 1; // => 
    // strlen() doesn't include the '\0' at the end of 'cmdLine', which should be saved to act as the end of the last token.

    char* current_char;
    int previous_was_char = 0; // used as a boolean indicator of a previous instance of char to battle the case in which -
    // we have multiple deliminating symbols in a row.

    for (size_t i = 0; i < len; i++) { // manually tokanizing cmdLine:
        current_char = &cmdLine[i];
        if ((*current_char == '\t') || 
            (*current_char == '\r') || 
            (*current_char == '\0') || 
            (*current_char == ' ')) { // identifying "non-chars".

            *current_char = '\0'; // =>
            // swapping the-('\t','\r','\0',' ') unsigned-chars with-'\0' for standardized parsing convenience in-C/C++.

            previous_was_char = 0;
            continue;

        } else {

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

            if (previous_was_char == 0) {
                tokens[current_token_count] = current_char; // this also accounts for the first token's occurrence.
                
                current_token_count++;
            }
            previous_was_char = 1;
        }
    }
    // resizing 'tokens''s heap memory if needed:
    //---
    if (current_token_count >= max_tokens - 1) {
        max_tokens += 1;
        char** temp = realloc(tokens, max_tokens * sizeof(char*));
        if (!temp) { // freeing the original tokens array if realloc fails and returning 'NULL' to indicate an error:
            free(tokens); 
            return NULL; 
        }
        tokens = temp;
    }
    //---
    tokens[current_token_count] = NULL;
    return tokens;
}


void executeCommand(char** info) {

    execvp(info[0], info); 
     

    // only gets here on failure..:
    fprintf(stderr, "Shell error: %s: %s (errno: %d).\n", info[0], strerror(errno), errno);
    // fprintf: for sending formatted output to a specific file stream,
    // a standard practice in shell enviroments...
    //
    // stderr: a separate stream from stdout, used primarily for error messages...

    // killing the child process immediately:
    _exit(EXIT_FAILURE);  // =>
    // _exit(): contrary to exit(), which is from the std c library, _exit() is a low -
    // level system call, it does not do any cleanup or flushes buffers...
    //
    // GEMINI's explanation:
    // Why use _exit() in a child process?
    // When you fork(), the child gets a copy of the parent's memory, including the printf buffers.
    // 1. If the child fails and calls the regular exit(), it might "flush" the parent's old data to
    // the screen a second time.
    // 2. This can lead to weird "double-printing" bugs in your shell's output.
    // 3. Since the child was supposed to be replaced by execv anyway, using _exit() is the safest
    // way to "vanish" without accidentally messing with the parent's terminal state.
}

void tree(){ 
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        print_tree(cwd, 0);
    } else {
        printf("Command error: 'tree' couldn't execute.\n");
    }
}

void print_tree(char* subpath, int level) {
    DIR* dir = opendir(subpath);
    if (dir == NULL) {
        return; // Not a directory or can't open
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // skip current and parent directory entries.
        }
        
        // Print indentation (4 spaces per level)
        for (int i = 0; i < level; i++) {
            printf("    ");
        }
        
        // Print branch symbol for non-root levels
        if (level > 0) {
            printf("├── ");
        }
        
        printf("%s\n", entry->d_name);

        // Check if entry is a directory using stat
        char entry_path[1024];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", subpath, entry->d_name);
        struct stat st;
        int is_dir = (stat(entry_path, &st) == 0 && S_ISDIR(st.st_mode));
        
        if (is_dir) { // if the entry is a directory:
            char new_subpath[1024];
            snprintf(new_subpath, sizeof(new_subpath), "%s/%s", subpath, entry->d_name);
            print_tree(new_subpath, level + 1); // recursive call
        }
    }
    
    closedir(dir);
}