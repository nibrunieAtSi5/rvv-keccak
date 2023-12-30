#include <riscv_vector.h>

#ifdef NO_ZVBB_INTRINSICS

/** This builtin was added recently to RVV intrinsics and seems to be missing from
 *  some recent clang version.
*/
static vuint64m4_t __riscv_vcreate_v_u64m1_u64m4(vuint64m1_t v0, vuint64m1_t v1, vuint64m1_t v2, vuint64m1_t v3) {
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
static vuint64m4_t __riscv_vrol_vx_u64m4(vuint64m4_t vs2, size_t rot, size_t vl){
    vuint64m4_t res = __riscv_vor_vv_u64m4(__riscv_vsll_vx_u64m4(vs2, rot,      vl),
                                           __riscv_vsrl_vx_u64m4(vs2, 64 - rot, vl),
                                           vl);
    return res;
}


static vuint64m4_t __riscv_vrol_vv_u64m4(vuint64m4_t data, vuint64m4_t rots, size_t vl){
    vuint64m4_t rotsComp = __riscv_vrsub_vx_u64m4(rots, 64, vl); 
    vuint64m4_t res = __riscv_vor_vv_u64m4(__riscv_vsll_vv_u64m4(data, rots,      vl),
                                           __riscv_vsrl_vv_u64m4(data, rotsComp, vl),
                                           vl);
    return res;
}

static vuint64m8_t __riscv_vrol_vv_u64m8(vuint64m8_t data, vuint64m8_t rots, size_t vl){
    vuint64m8_t rotsComp = __riscv_vrsub_vx_u64m8(rots, 64, vl); 
    vuint64m8_t res = __riscv_vor_vv_u64m8(__riscv_vsll_vv_u64m8(data, rots,      vl),
                                           __riscv_vsrl_vv_u64m8(data, rotsComp, vl),
                                           vl);
    return res;
}

static vuint64m4_t __riscv_vandn_vv_u64m4(vuint64m4_t vs2, vuint64m4_t vs1, size_t vl){
    vuint64m4_t res = __riscv_vand_vv_u64m4(__riscv_vnot_v_u64m4(vs1, vl),
                                            vs2,
                                            vl);
    return res;
}
#endif // ifdef NO_ZVBB_INTRINSICS

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

// Keccak 24 round constants for ι step
static const uint64_t RC[25] = {
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