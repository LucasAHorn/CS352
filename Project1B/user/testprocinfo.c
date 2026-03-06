#include "kernel/types.h"
#include "user/user.h"

int main() {
    struct proc_info info;

    if(getprocinfo(&info) < 0){
        printf("getprocinfo failed\n");
        exit(1);
    }

    printf("PID: %d\n", info.pid);
    printf("PPID: %d\n", info.ppid);
    printf("STATE: %d\n", info.state);
    printf("MEM SIZE: %lu\n", info.sz); 

    exit(0);
}