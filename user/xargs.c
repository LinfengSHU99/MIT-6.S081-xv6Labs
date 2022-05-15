#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int fd = 0;
    char *exec_name = argv[1];
    // printf("exec_name = %s\n", exec_name);
    char read_argv[MAXARG] = {0};
    char *new_argv[MAXARG];
    new_argv[0] = exec_name;
    for (int i = 2; i < argc; i++) {
        new_argv[i - 1] = argv[i];
    }
    char c = 0;
    int top = 0;
    while (read(fd, &c, sizeof(char)) != 0) {
        if (c != '\n') {
            read_argv[top++] = c;
        }
        else {
            // read_argv[top++] = 0;
            new_argv[argc - 1] = read_argv;
            // for (int i = 0; i < argc; i++) {
                // printf("new_argv %d = %s\n", i, new_argv[i]);
            // }
            int r = fork();
            if (r == 0) {
                exec(exec_name, new_argv);
                exit(0);
            }
            else {
                wait(0);
                top = 0;
                memset(read_argv, 0, sizeof(char) * MAXARG);
            }
        }
    }
    exit(0);
}