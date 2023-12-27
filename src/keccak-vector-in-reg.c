#include <riscv_vector.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#ifdef NO_ZVBB_INTRINSICS

/** This builtin was added recently to RVV intrinsics and seems to be missing from
 *  some recent clang version.
*/
vuint64m4_t __riscv_vcreate_v_u64m1_u64m4(vuint64m1_t v0, vuint64m1_t v1, vuint64m1_t v2, vuint64m1_t v3) {
    vuint64m4_t res = __riscv_vundefined_u64m4();
    res = __riscv_vset_v_u64m1_u64m4(res, 0, v0);
    res = __riscv_vset_v_u64m1_u64m4(res, 1, v1);
    res = __riscv_vset_v_u64m1_u64m4(res, 2, v2);
    res = __riscv_vset_v_u64m1_u64m4(res, 3, v3);
    return res;
}

/** This builtin was added recently to RVV intrinsics and seems to be missing from
 *  some recent clang version.
*/
vuint64m4_t __riscv_vrol_vx_u64m4(vuint64m4_t vs2, size_t rot, size_t vl){
    vuint64m4_t res = __riscv_vor_vv_u64m4(__riscv_vsll_vx_u64m4(vs2, rot,      vl),
                                           __riscv_vsrl_vx_u64m4(vs2, 64 - rot, vl),
                                           vl);
    return res;
}


vuint64m4_t __riscv_vrol_vv_u64m4(vuint64m4_t data, vuint64m4_t rots, size_t vl){
    vuint64m4_t rotsComp = __riscv_vrsub_vx_u64m4(rots, 64, vl); 
    vuint64m4_t res = __riscv_vor_vv_u64m4(__riscv_vsll_vv_u64m4(data, rots,      vl),
                                           __riscv_vsrl_vv_u64m4(data, rotsComp, vl),
                                           vl);
    return res;
}
#endif

// round constants for ι step
const uint64_t RC[25] = {
    0x0000000000000001, // RC[0]	
    0x0000000000008082, // RC[1]	
    0x800000000000808A, // RC[2]	
    0x8000000080008000, // RC[3]	
    0x000000000000808B, // RC[4]	
    0x0000000080000001, // RC[5]	
    0x8000000080008081, // RC[6]	
    0x8000000000008009, // RC[7]	
    0x000000000000008A, // RC[8]	
    0x0000000000000088, // RC[9]	
    0x0000000080008009, // RC[10]
    0x000000008000000A, // RC[11]
    0x000000008000808B, // RC[12]
    0x800000000000008B, // RC[13]
    0x8000000000008089, // RC[14]
    0x8000000000008003, // RC[15]
    0x8000000000008002, // RC[16]
    0x8000000000000080, // RC[17]
    0x000000000000800A, // RC[18]
    0x800000008000000A, // RC[19]
    0x8000000080008081, // RC[20]
    0x8000000000008080, // RC[21] 
    0x0000000080000001, // RC[22]
    0x8000000080008008, // RC[23]
};


/** return the value of the instret counter
 *
 *  The instret counter counts the number of retired (executed) instructions.
*/
static unsigned long read_instret(void)
{
  unsigned long instret;
  asm volatile ("rdinstret %0" : "=r" (instret));
  return instret;
}


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
    // initial loading of 5x5x64 state
    vuint64m4_t row0 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 0, 5);
    vuint64m4_t row1 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 5, 5);
    vuint64m4_t row2 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 10, 5);
    vuint64m4_t row3 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 15, 5);
    vuint64m4_t row4 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 20, 5);

    for(round=0; round<24; round++)
    {
        /* === θ step (see [Keccak Reference, Section 2.3.2]) === */
        {
            vuint64m4_t C_01 = __riscv_vxor_vv_u64m4(row0, row1, 5);
            vuint64m4_t C_23 = __riscv_vxor_vv_u64m4(row2, row3, 5);
            vuint64m4_t C_014 = __riscv_vxor_vv_u64m4(C_01, row4, 5);
            vuint64m4_t C_vector = __riscv_vxor_vv_u64m4(C_23, C_014, 5);

            /* Compute the θ effect for all the columns */
            vuint64m4_t C_4_ext = __riscv_vslidedown_vx_u64m4(C_vector, 4, 1);       // {C[4]}
            vuint64m4_t D_opHi = __riscv_vslideup_vx_u64m4(C_4_ext, C_vector, 1, 5); // {C[4],    C[0], C[1], C[2], C[3]}

            vuint64m4_t D_opLo = __riscv_vslidedown_vx_u64m4(C_vector, 1, 5); // {C[1], C[2], C[3], C[4], 0}
            D_opLo = __riscv_vslideup_vx_u64m4(D_opLo, C_vector, 4, 5);       // {C[1], C[2], C[3], C[4], C[0]}
            D_opLo = __riscv_vrol_vx_u64m4(D_opLo, 1, 5);
            vuint64m4_t D = __riscv_vxor_vv_u64m4(D_opLo, D_opHi, 5);
            
            /* Apply the θ effect */
            row0 = __riscv_vxor_vv_u64m4(row0, D, 5);
            row1 = __riscv_vxor_vv_u64m4(row1, D, 5);
            row2 = __riscv_vxor_vv_u64m4(row2, D, 5);
            row3 = __riscv_vxor_vv_u64m4(row3, D, 5);
            row4 = __riscv_vxor_vv_u64m4(row4, D, 5);

            __riscv_vse64_v_u64m4((uint64_t*)state,      row0, 5);
            __riscv_vse64_v_u64m4((uint64_t*)state + 5,  row1, 5);
            __riscv_vse64_v_u64m4((uint64_t*)state + 10, row2, 5);
            __riscv_vse64_v_u64m4((uint64_t*)state + 15, row3, 5);
            __riscv_vse64_v_u64m4((uint64_t*)state + 20, row4, 5);

        }

        /* === ρ and π steps (see [Keccak Reference, Sections 2.3.3 and 2.3.4]) === */
        {
            // indices and rotations generated with script/utils.py
            uint16_t offsetByte_AtoB[] = {
                // byte offset for each index
                0, 48, 96, 144, 192, 
                24, 72, 80, 128, 176, 
                8, 56, 104, 152, 160, 
                32, 40, 88, 136, 184, 
                16, 64, 112, 120, 168, 
            };
            uint16_t offsetIndex_AtoB[] = {
                // index offset for each index
                0, 6, 12, 18, 24, 
                3, 9, 10, 16, 22, 
                1, 7, 13, 19, 20, 
                4, 5, 11, 17, 23, 
                2, 8, 14, 15, 21, 
            };
            uint64_t rotation_B[] = {
                0, 44, 43, 21, 14, 
                28, 20, 3, 45, 61, 
                1, 6, 25, 8, 18, 
                27, 36, 10, 15, 56, 
                62, 55, 39, 41, 2, 
            };

            // The following assumes VLEN >= 128, and uses 2x 8-register groups to load/transpose 
            // matrix A to B
            // First 16 elements [0...15]
            vuint16m2_t B_index_0 = __riscv_vle16_v_u16m2(offsetByte_AtoB, 16);
            vuint64m8_t B_0 = __riscv_vluxei16_v_u64m8(state, B_index_0, 16);
            vuint64m8_t B_rots_0 = __riscv_vle64_v_u64m8(rotation_B, 16);
            B_0 = __riscv_vrol_vv_u64m8(B_0, B_rots_0, 16);
            // Last 9 elements [16...24]
            vuint16m2_t B_index_1 = __riscv_vle16_v_u16m2(offsetByte_AtoB + 16, 9);
            vuint64m8_t B_1 = __riscv_vluxei16_v_u64m8(state, B_index_1, 9);
            vuint64m8_t B_rots_1 = __riscv_vle64_v_u64m8(rotation_B + 16, 9);
            B_1 = __riscv_vrol_vv_u64m8(B_1, B_rots_1, 9);
            // To avoid state corruption, B partial results can only be stored once indexed load accesses
            // have all been completed.
            __riscv_vse64_v_u64m8((uint64_t*)state, B_0, 16);
            __riscv_vse64_v_u64m8((uint64_t*)state + 16, B_1, 9);


        }
        /* === χ step (see [Keccak Reference, Section 2.3.1]) === */
        {
            row0 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 0 * 5, 5);
            vuint64m4_t row0_xp1 = __riscv_vslidedown_vx_u64m4(row0, 1, 4);   // {row[1], row[2], row[3], row{4}}
            vuint64m4_t row0_xp2 = __riscv_vslidedown_vx_u64m4(row0, 2, 3);   // {row[2], row[3], row[4]}
            row0_xp1 = __riscv_vslideup_vx_u64m4(row0_xp1, row0, 4, 5); // {row[1], row[2], row[3], row[4], row[0]}
            row0_xp2 = __riscv_vslideup_vx_u64m4(row0_xp2, row0, 3, 5); // {row[2], row[3], row[4], row[0], row[1]}
            row0 = __riscv_vxor_vv_u64m4(row0, __riscv_vandn_vv_u64m4(row0_xp2, row0_xp1, 5), 5);

            /* === ι step (see [Keccak Reference, Section 2.3.5]) === */
            row0 = __riscv_vxor_vx_u64m4_tu(row0, row0, RC[round], 1);

            row1 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 1 * 5, 5);
            vuint64m4_t row1_xp1 = __riscv_vslidedown_vx_u64m4(row1, 1, 4);   // {row[1], row[2], row[3], row{4}}
            vuint64m4_t row1_xp2 = __riscv_vslidedown_vx_u64m4(row1, 2, 3);   // {row[2], row[3], row[4]}
            row1_xp1 = __riscv_vslideup_vx_u64m4(row1_xp1, row1, 4, 5); // {row[1], row[2], row[3], row[4], row[0]}
            row1_xp2 = __riscv_vslideup_vx_u64m4(row1_xp2, row1, 3, 5); // {row[2], row[3], row[4], row[0], row[1]}
            row1 = __riscv_vxor_vv_u64m4(row1, __riscv_vandn_vv_u64m4(row1_xp2, row1_xp1, 5), 5);

            row2 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 2 * 5, 5);
            vuint64m4_t row2_xp1 = __riscv_vslidedown_vx_u64m4(row2, 1, 4);   // {row[1], row[2], row[3], row{4}}
            vuint64m4_t row2_xp2 = __riscv_vslidedown_vx_u64m4(row2, 2, 3);   // {row[2], row[3], row[4]}
            row2_xp1 = __riscv_vslideup_vx_u64m4(row2_xp1, row2, 4, 5); // {row[1], row[2], row[3], row[4], row[0]}
            row2_xp2 = __riscv_vslideup_vx_u64m4(row2_xp2, row2, 3, 5); // {row[2], row[3], row[4], row[0], row[1]}
            row2 = __riscv_vxor_vv_u64m4(row2, __riscv_vandn_vv_u64m4(row2_xp2, row2_xp1, 5), 5);

            row3 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 3 * 5, 5);
            vuint64m4_t row3_xp1 = __riscv_vslidedown_vx_u64m4(row3, 1, 4);   // {row[1], row[2], row[3], row{4}}
            vuint64m4_t row3_xp2 = __riscv_vslidedown_vx_u64m4(row3, 2, 3);   // {row[2], row[3], row[4]}
            row3_xp1 = __riscv_vslideup_vx_u64m4(row3_xp1, row3, 4, 5); // {row[1], row[2], row[3], row[4], row[0]}
            row3_xp2 = __riscv_vslideup_vx_u64m4(row3_xp2, row3, 3, 5); // {row[2], row[3], row[4], row[0], row[1]}
            row3 = __riscv_vxor_vv_u64m4(row3, __riscv_vandn_vv_u64m4(row3_xp2, row3_xp1, 5), 5);

            row4 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 4 * 5, 5);
            vuint64m4_t row4_xp1 = __riscv_vslidedown_vx_u64m4(row4, 1, 4);   // {row[1], row[2], row[3], row{4}}
            vuint64m4_t row4_xp2 = __riscv_vslidedown_vx_u64m4(row4, 2, 3);   // {row[2], row[3], row[4]}
            row4_xp1 = __riscv_vslideup_vx_u64m4(row4_xp1, row4, 4, 5); // {row[1], row[2], row[3], row[4], row[0]}
            row4_xp2 = __riscv_vslideup_vx_u64m4(row4_xp2, row4, 3, 5); // {row[2], row[3], row[4], row[0], row[1]}
            row4 = __riscv_vxor_vv_u64m4(row4, __riscv_vandn_vv_u64m4(row4_xp2, row4_xp1, 5), 5);
        }
    }
    {
        // final storing of 5x5x64 state
        __riscv_vse64_v_u64m4((uint64_t*)state + 0 , row0, 5);
        __riscv_vse64_v_u64m4((uint64_t*)state + 5 , row1, 5);
        __riscv_vse64_v_u64m4((uint64_t*)state + 10, row2, 5);
        __riscv_vse64_v_u64m4((uint64_t*)state + 15, row3, 5);
        __riscv_vse64_v_u64m4((uint64_t*)state + 20, row4, 5);

    }
    stop = read_instret();
    long cycleCnt = (stop - start);
    nCalls += 24;
    totalEvts += cycleCnt;
    if (cycleCnt < minLatency) minLatency = cycleCnt;
    if (cycleCnt > maxLatency) maxLatency = cycleCnt;
}