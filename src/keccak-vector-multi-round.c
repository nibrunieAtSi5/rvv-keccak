#include <riscv_vector.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <keccak-vector-common.h>



extern unsigned long totalEvts, nCalls, minLatency, maxLatency;

void KeccakF1600_StatePermute_vector(void *state)
{
    unsigned int round;

    unsigned long start, stop;
    start = read_instret();

    // initial state loading
    vuint64m4_t row0 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 0, 5);
    vuint64m4_t row1 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 5, 5);
    vuint64m4_t row2 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 10, 5);
    vuint64m4_t row3 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 15, 5);
    vuint64m4_t row4 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 20, 5);

    for(round=0; round<24; round++) {
        {   /* === θ step (see [Keccak Reference, Section 2.3.2]) === */
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
            uint16_t offset_AtoB[] = {
                // byte offset for each index
                0, 48, 96, 144, 192, 
                24, 72, 80, 128, 176, 
                8, 56, 104, 152, 160, 
                32, 40, 88, 136, 184, 
                16, 64, 112, 120, 168, 
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
            vuint16m2_t B_index_0 = __riscv_vle16_v_u16m2(offset_AtoB, 16);
            vuint64m8_t B_0 = __riscv_vluxei16_v_u64m8(state, B_index_0, 16);
            vuint64m8_t B_rots_0 = __riscv_vle64_v_u64m8(rotation_B, 16);
            B_0 = __riscv_vrol_vv_u64m8(B_0, B_rots_0, 16);
            // Last 9 elements [16...24]
            vuint16m2_t B_index_1 = __riscv_vle16_v_u16m2(offset_AtoB + 16, 9);
            vuint64m8_t B_1 = __riscv_vluxei16_v_u64m8(state, B_index_1, 9);
            vuint64m8_t B_rots_1 = __riscv_vle64_v_u64m8(rotation_B + 16, 9);
            B_1 = __riscv_vrol_vv_u64m8(B_1, B_rots_1, 9);
            // To avoid state corruption, B partial results can only be stored once indexed load accesses
            // have all been completed.
            __riscv_vse64_v_u64m8((uint64_t*)state, B_0, 16);
            __riscv_vse64_v_u64m8((uint64_t*)state + 16, B_1, 9);
        }

        {
            row0 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 0, 5);
            row1 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 5, 5);
            row2 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 10, 5);
            row3 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 15, 5);
            row4 = __riscv_vle64_v_u64m4(((uint64_t*)state) + 20, 5);
        }

        /* === χ step (see [Keccak Reference, Section 2.3.1]) === */
        {
            const uint16_t xp1_indices_buf[5] = {1, 2, 3, 4, 0};
            const uint16_t xp2_indices_buf[5] = {2, 3, 4, 0, 1};
            vuint16m1_t xp1_indices = __riscv_vle16_v_u16m1(xp1_indices_buf, 5); 
            vuint16m1_t xp2_indices = __riscv_vle16_v_u16m1(xp2_indices_buf, 5); 
            vuint64m4_t row0_xp1 = __riscv_vrgatherei16_vv_u64m4(row0, xp1_indices, 5);
            vuint64m4_t row0_xp2 = __riscv_vrgatherei16_vv_u64m4(row0, xp2_indices, 5);
            row0 = __riscv_vxor_vv_u64m4(row0, __riscv_vandn_vv_u64m4(row0_xp2, row0_xp1, 5), 5);

            /* === ι step (see [Keccak Reference, Section 2.3.5]) === */
            row0 = __riscv_vxor_vx_u64m4_tu(row0, row0, RC[round], 1);

            vuint64m4_t row1_xp1 = __riscv_vrgatherei16_vv_u64m4(row1, xp1_indices, 5);
            vuint64m4_t row1_xp2 = __riscv_vrgatherei16_vv_u64m4(row1, xp2_indices, 5);
            row1 = __riscv_vxor_vv_u64m4(row1, __riscv_vandn_vv_u64m4(row1_xp2, row1_xp1, 5), 5);

            vuint64m4_t row2_xp1 = __riscv_vrgatherei16_vv_u64m4(row2, xp1_indices, 5);
            vuint64m4_t row2_xp2 = __riscv_vrgatherei16_vv_u64m4(row2, xp2_indices, 5);
            row2 = __riscv_vxor_vv_u64m4(row2, __riscv_vandn_vv_u64m4(row2_xp2, row2_xp1, 5), 5);

            vuint64m4_t row3_xp1 = __riscv_vrgatherei16_vv_u64m4(row3, xp1_indices, 5);
            vuint64m4_t row3_xp2 = __riscv_vrgatherei16_vv_u64m4(row3, xp2_indices, 5);
            row3 = __riscv_vxor_vv_u64m4(row3, __riscv_vandn_vv_u64m4(row3_xp2, row3_xp1, 5), 5);

            vuint64m4_t row4_xp1 = __riscv_vrgatherei16_vv_u64m4(row4, xp1_indices, 5);
            vuint64m4_t row4_xp2 = __riscv_vrgatherei16_vv_u64m4(row4, xp2_indices, 5);
            row4 = __riscv_vxor_vv_u64m4(row4, __riscv_vandn_vv_u64m4(row4_xp2, row4_xp1, 5), 5);
        }
    }

    // final state storing (after 24-round execution)
    __riscv_vse64_v_u64m4((uint64_t*)state,      row0, 5);
    __riscv_vse64_v_u64m4((uint64_t*)state + 5,  row1, 5);
    __riscv_vse64_v_u64m4((uint64_t*)state + 10, row2, 5);
    __riscv_vse64_v_u64m4((uint64_t*)state + 15, row3, 5);
    __riscv_vse64_v_u64m4((uint64_t*)state + 20, row4, 5);

    stop = read_instret();
    long cycleCnt = (stop - start);
    nCalls += 24;
    totalEvts += cycleCnt;
    if (cycleCnt < minLatency) minLatency = cycleCnt;
    if (cycleCnt > maxLatency) maxLatency = cycleCnt;
}