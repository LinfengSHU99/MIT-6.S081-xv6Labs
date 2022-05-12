#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    char b = '~';
    write(fd[1], &b, 1);
    int r = fork();
    char byte[1] = {0};
    if (r == 0) {
        read(fd[0], byte, 1);
        printf("%d: received ping\n", getpid());
        write(fd[1], byte, 1);
        exit(0);
    }
    else {
        wait(0);
        read(fd[0], byte, 1);
        printf("%d: received pong\n", getpid());
        exit(0);
    }
}