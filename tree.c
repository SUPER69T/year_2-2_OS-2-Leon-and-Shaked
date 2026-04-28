#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// Standalone tree printing function
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
    if (dir == NULL) return; // Not a directory or can't open
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // skip current and parent directory entries.
        }
        
        // Print indentation (4 spaces per level):
        for (int i = 0; i < level; i++) printf("    ");
        
        // Print branch symbol for non-root levels:
        if (level > 0) printf("├── ");
        
        printf("%s\n", entry->d_name);

        // Check if entry is a directory using stat
        char entry_path[2048];
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

int main() {
    tree();
    return 0;
}