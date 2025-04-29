#include <riscv_vector.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#include "keccak-vector-common.h"

#include "bench_utils.h"


/** Declaration of RVV implementation of Keccak-F1600 round function 
 *
 * @param state input/output 1600-bit state 
 * @param round round index (must verify 0 <= round < 24)
 *
*/
void KeccakF1600_Round_vector(void *state, unsigned round);

extern unsigned long totalEvts, nCalls, minPerfCount, maxPerfCount;

void KeccakF1600_StatePermute_vector(void *state)
{
    unsigned int round;

    //for(round=0; round<24; round++) {
        unsigned long start, stop;
        start = read_perf_counter();
        KeccakF1600_Round_vector(state, 0);
        stop = read_perf_counter();
        long perfCnt = (stop - start);
        nCalls++;
        totalEvts += perfCnt;
        if (perfCnt < minPerfCount) minPerfCount = perfCnt;
        if (perfCnt > maxPerfCount) maxPerfCount = perfCnt;

    //}
}