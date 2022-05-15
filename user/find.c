#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

#define MAX_NAME_SIZE 100
void search(char *file_name, char *dir_name) {
    struct stat st;
    struct dirent de;
    int fd = 0;
    fd = open(dir_name, 0);
    if (fd < 0) {
        printf("%s\n", dir_name);
        printf("open error!\n");
        close(fd);
        return;
    }
    else {
        int r = fstat(fd, &st);
        if (r < 0) {
            printf("stat error!\n");
            close(fd);
            return;
        }
        else if (st.type == T_FILE || st.type == T_DEVICE) {
            printf("%s\n", dir_name);
            printf("first arg should be dir!\n");
            close(fd);
            return;
        }
        else if (st.type == T_DIR) {
            while (read(fd, &de, sizeof(de)) == sizeof(de)) {
                if (de.inum == 0) continue;
                if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
                        continue;
                    }
                char dn[MAX_NAME_SIZE] = {0};
                strcpy(dn, dir_name);
                strcpy(dn + strlen(dir_name), "/");
                strcpy(dn + strlen(dn), de.name);
                // printf("dn = %s\n", dn);
                int fd2 = open(dn, 0);
                fstat(fd2, &st);
                close(fd2);
                // printf("fd2 = %d, type = %d\n", fd2, st.type);
                if (st.type == T_DIR) {
                    
                    
                        // printf("reach here  fn: %s dn:%s\n",file_name, dn);
                        search(file_name, dn);
                    
                }
                
                else if (strcmp(de.name, file_name) == 0 && st.type == T_FILE) {
                    printf("%s/%s\n", dir_name, file_name);
                }
            }
            return; 
        }
    }
}

int main(int argc, char *argv[]) {
    // printf("filename = %s\ndirname = %s\n", argv[1], argv[2]);
    search(argv[2], argv[1]);
    exit(0);
}