#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

void
check(char *label, int before, int after, int expect_increase) {
    if(expect_increase && after <= before) {
        printf("FAIL [%s]: expected increase, got before=%d after=%d\n", label, before, after);
    } else if(!expect_increase && after != before) {
        printf("FAIL [%s]: expected no change, got before=%d after=%d\n", label, before, after);
    } else {
        printf("PASS [%s]: before=%d after=%d\n", label, before, after);
    }
}

int
main() {
    struct resource_usage before, after;
    int pid;

    printf("=== testusage2: extended resource usage tests ===\n\n");

    // --- Test 1: syscallCount increases with syscalls ---
    printf("-- Test 1: syscallCount increases --\n");
    pid = getresourceusage(&before);
    // make some explicit syscalls
    getpid();
    getpid();
    getpid();
    getpid();
    getpid();
    getresourceusage(&after);
    check("syscallCount", before.syscallCount, after.syscallCount, 1);

    // --- Test 2: sleepCount increases after pause ---
    printf("\n-- Test 2: sleepCount increases after pause --\n");
    getresourceusage(&before);
    pause(1);
    pause(1);
    pause(1);
    getresourceusage(&after);
    check("sleepCount", before.sleepCount, after.sleepCount, 1);
    int sleep_delta = after.sleepCount - before.sleepCount;
    if(sleep_delta == 3) {
        printf("PASS [sleepCount delta]: got exactly 3 sleeps\n");
    } else {
        printf("FAIL [sleepCount delta]: expected 3, got %d\n", sleep_delta);
    }

    // --- Test 3: contextSwitches increases after yielding ---
    printf("\n-- Test 3: contextSwitches increase after pausing --\n");
    getresourceusage(&before);
    pause(2);
    getresourceusage(&after);
    check("contextSwitches", before.contextSwitches, after.contextSwitches, 1);

    // --- Test 4: cpuTicks increases during CPU-bound work ---
    printf("\n-- Test 4: cpuTicks increase during busy work --\n");
    getresourceusage(&before);
    // spin for a while to burn CPU time and trigger timer interrupts
    volatile int x = 0;
    for(int i = 0; i < 50000000; i++) x += i;
    getresourceusage(&after);
    check("cpuTicks", before.cpuTicks, after.cpuTicks, 1);
    printf("  (busy loop result: x=%d to prevent optimization)\n", x);

    // --- Test 5: pid returned matches getpid() ---
    printf("\n-- Test 5: returned pid matches getpid() --\n");
    pid = getresourceusage(&after);
    int mypid = getpid();
    if(pid == mypid) {
        printf("PASS [pid match]: getresourceusage returned pid=%d, getpid=%d\n", pid, mypid);
    } else {
        printf("FAIL [pid match]: getresourceusage returned pid=%d, getpid=%d\n", pid, mypid);
    }

    // --- Test 6: values don't decrease ---
    printf("\n-- Test 6: counters never decrease --\n");
    getresourceusage(&before);
    pause(1);
    getpid(); getpid();
    getresourceusage(&after);
    int ok = 1;
    if(after.cpuTicks        < before.cpuTicks)        { printf("FAIL [cpuTicks decreased]\n");        ok = 0; }
    if(after.syscallCount    < before.syscallCount)    { printf("FAIL [syscallCount decreased]\n");    ok = 0; }
    if(after.contextSwitches < before.contextSwitches) { printf("FAIL [contextSwitches decreased]\n"); ok = 0; }
    if(after.sleepCount      < before.sleepCount)      { printf("FAIL [sleepCount decreased]\n");      ok = 0; }
    if(ok) printf("PASS [no counters decreased]\n");

    printf("\n=== testusage2 done ===\n");
    exit(0);
}