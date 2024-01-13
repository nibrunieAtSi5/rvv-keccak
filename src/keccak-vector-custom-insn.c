#include <riscv_vector.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include <keccak-vector-common.h>


extern unsigned long totalEvts, nCalls, minLatency, maxLatency;

/**
 * Function that computes 24 Keccak-f[1600] permutation rounds on the given state.
 * original from: https://github.com/XKCP/XKCP/blob/master/Standalone/CompactFIPS202/C/Keccak-readable-and-compact.c
 */
void KeccakF1600_StatePermute_vector(void *state)
{
    unsigned int round;

    unsigned long start, stop;
    start = read_instret();
    unsigned long vl = 32;

    __asm volatile (
        "vsetivli x0, 25, e64, m8, tu, mu\n"
        "vle64.v v8, 0(%[state])\n"
        "vsetvli %[vl], %[vl], e64, m8, tu, mu\n"
        // .insn r opc, func3, func7, vd, vs1, vs2
        ".insn r 0x77, 0x2, 0x53, x8, x17, x24\n"
        "vsetivli x0, 25, e64, m8, tu, mu\n"
        "vse64.v v8, 0(%[state])\n"
        : [vl]"+r"(vl)
        : [state]"r"(state)
        : 
    );

    stop = read_instret();
    long cycleCnt = (stop - start);
    nCalls += 24;
    totalEvts += cycleCnt;
    if (cycleCnt < minLatency) minLatency = cycleCnt;
    if (cycleCnt > maxLatency) maxLatency = cycleCnt;
}