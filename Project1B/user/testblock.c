#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
    int pid = fork();

    if (pid < 0) {
        printf("fork failed\n");
        exit(1);
    }

    if (pid == 0) {
        // child
        for(int i = 0; i < 5; i++){
            printf("child running\n");
            pause(1);  // xv6 sleep syscall
        }
        exit(0);
    } else {
        // parent
        pause(2); // let child run a bit
        printf("parent blocking child\n");
        blockchild(pid);

        printf("parent sleeping for 5 ticks\n");
        pause(5); // child should not print during this time

        printf("parent unblocking child\n");
        unblockchild(pid);

        wait(0); // wait for child to exit
        exit(0);
    }
}