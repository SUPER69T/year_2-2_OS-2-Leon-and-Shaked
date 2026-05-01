#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// Standalone tree printing function:
void print_tree(char* subpath, int level);

void tree() { 
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        print_tree(cwd, 0);
    } else {
        perror("getcwd error");
    }
}

void print_tree(char* subpath, int level) { 
    DIR* dir = opendir(subpath);
    if (dir == NULL) return; // Not a directory or can't open.
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            // entry->d_name="."/"..":
            // every directory in linux is auto-injected with 2 directories named ".", and "..":
            // "."-directory: encodes the Inode of the current directory, ".."-directory encodes the -
            // Inode of the parent directory of the "." directory. usually these directories are hidden -
            // when using the ls/la commands, but are hardcoded by the operating system to be used by -
            // command that require general access, based on the current directory.
            // in our case we have no use for these directories in this code so we simply skip them.
            continue;
        }
        
        // Print indentation (4 spaces per level):
        for (int i = 0; i < level; i++) printf("    ");
        
        // Print branch symbol for non-root levels:
        if (level > 0) printf("├───");
        
        printf("%s\n", entry->d_name);

        // Checking whether entry is a directory using stat:
        //---
        char entry_path[2048];
        snprintf(entry_path, sizeof(entry_path), "%s/%s", subpath, entry->d_name);
        struct stat st;
        int is_dir = (stat(entry_path, &st) == 0 && S_ISDIR(st.st_mode));
        //---

        if (is_dir) { // if the entry is a directory:
            char new_subpath[1024];
            snprintf(new_subpath, sizeof(new_subpath), "%s/%s", subpath, entry->d_name);
            print_tree(entry_path, level + 1); // recursive call
            printf("\n"); // simply to de-clutter the output (purely cosmetic).
        }
    }
    
    closedir(dir);
}

int main() {
    tree();
    return 0;
}