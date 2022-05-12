#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 1) {
        printf("Argument Error!!!\n");
        exit(1);
    }

    int tick = atoi(argv[0]);
    int r = sleep(tick);
    exit(r);
}