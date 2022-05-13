#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    int fd[2];
    pipe(fd);
    int num = 2;
    for (int i = 2; i <= 35; i++) {
        write(fd[0], &i, 4);
    }
    int r = fork();
    if (r != 0) {
        wait(0);
        
    }
    int fd2[2];
    int cnt = 0;
    fd2[1] = fd[1];
    // int r = fork();
    child:
    // int cnt = 0;
    // int fd2[2];
    fd[1] = fd2[1];

    cnt = 0;
    pipe(fd2);
    // if (r == 0) {
        while (read(fd[1], &num, 4)) {
            int prime = 0;
            if (cnt == 0 && num == 35) {
                printf("prime %d\n", num);
                exit(0);
            }
            else {
                if (cnt == 0) {
                    printf("prime %d\n", num);
                    prime = num;
                    // fd[1] = fd2[1];
                    int ret = fork();
                    if (ret == 0) goto child;
                }
                else if (cnt > 0 && num % prime != 0) {
                    write(fd2[0], &num, 4);
                }
                
            }
            cnt++;
        }
        close(fd2[0]);
        close(fd[1]);
        int status = wait(0);
        exit(status);
    // }
    // else {

    // }
    
}