#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    int num = 2;
    for (int i = 2; i <= 99; i++) {
        write(fd[1], &i, 4);
    }
    close(fd[1]);
    int r = fork();
    if (r != 0) {
        wait(0);
        // exit(0);
    }
    int fd2[2];
    int cnt = 0;
    fd2[0] = fd[0];
    // int r = fork();
    child:
    // int cnt = 0;
    // int fd2[2];
    fd[0] = fd2[0];

    cnt = 0;
    pipe(fd2);
    int prime = 0;
    // if (r == 0) {
        while (read(fd[0], &num, 4)) {
            if (num == -1) break;
            if (cnt == 0 && num == 35) {
                printf("prime %d  cnt = 0 pid = %d\n", num, getpid());
                exit(0);
            }
            else {
                if (cnt == 0) {
                    printf("prime %d pid = %d\n", num, getpid());
                    prime = num;
                    // fd[1] = fd2[1];
                    int ret = fork();
                    if (ret == 0) goto child;
                }
                else if (cnt > 0 && num % prime != 0) {
                    write(fd2[1], &num, 4);
                }
                
            }
            cnt++;
        }
        int finish = -1;
        write(fd2[1], &finish, 4);
        close(fd2[1]);
        close(fd[0]);
        int status = wait(0);
        exit(status);
    // }
    // else {

    // }
    
}