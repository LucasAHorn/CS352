#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

int
main() {
    struct resource_usage usage;

    printf("=== Testing getresourceusage() ===\n");

    int pid = getresourceusage(&usage);
    if(pid < 0){
        printf("getresourceusage failed\n");
        exit(1);
    }

    printf("Resource usage for pid=%d:\n", pid);
    printf(" cpuTicks        = %d\n", usage.cpuTicks);
    printf(" syscallCount    = %d\n", usage.syscallCount);
    printf(" contextSwitches = %d\n", usage.contextSwitches);
    printf(" sleepCount      = %d\n", usage.sleepCount);

    printf("Doing some work to increase counters...\n");

    for(int i=0; i<5; i++){
        printf("Loop %d, pid=%d\n", i, getpid());
        pause(1);  // sleep 1 tick
    }

    printf("=== Checking updated resource usage ===\n");
    pid = getresourceusage(&usage);
    if(pid < 0){
        printf("getresourceusage failed\n");
        exit(1);
    }

    printf("Resource usage for pid=%d:\n", pid);
    printf(" cpuTicks        = %d\n", usage.cpuTicks);
    printf(" syscallCount    = %d\n", usage.syscallCount);
    printf(" contextSwitches = %d\n", usage.contextSwitches);
    printf(" sleepCount      = %d\n", usage.sleepCount);

    exit(0);
}