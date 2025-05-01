	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_v1p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0_zbc1p0_zve32f1p0_zve32x1p0_zve64d1p0_zve64f1p0_zve64x1p0_zvl128b1p0_zvl32b1p0_zvl64b1p0"
	.file	"keccak-vector.c"
	.globl	KeccakF1600_Round_vector        # -- Begin function KeccakF1600_Round_vector
	.p2align	1
	.type	KeccakF1600_Round_vector,@function
KeccakF1600_Round_vector:               # @KeccakF1600_Round_vector
# %bb.0:
	vsetivli	zero, 5, e64, m4, ta, ma
	// loading state
	vle64.v	v8, (a0)
	addi	a5, a0, 40
	vle64.v	v12, (a5)
	addi	t0, a0, 80
	vle64.v	v16, (t0)
	addi	a7, a0, 120
	vle64.v	v20, (a7)
	addi	a6, a0, 160
	vle64.v	v24, (a6)

	// computing C

    // u64 C_0= A_0_0 ^ A_0_1 ^ A_0_2 ^ A_0_3 ^ A_0_4;
    // u64 C_1= A_1_0 ^ A_1_1 ^ A_1_2 ^ A_1_3 ^ A_1_4;
    // u64 C_2= A_2_0 ^ A_2_1 ^ A_2_2 ^ A_2_3 ^ A_2_4;
    // u64 C_3= A_3_0 ^ A_3_1 ^ A_3_2 ^ A_3_3 ^ A_3_4;
    // u64 C_4= A_4_0 ^ A_4_1 ^ A_4_2 ^ A_4_3 ^ A_4_4;
	vsetivli	zero, 4, e64, m2, ta, ma
	// C[0:3] stored in v2v3
	vxor.vv	v0, v8, v12
	vxor.vv	v1, v16, v20
	vxor.vv	v2, v0, v24
	vxor.vv	v2, v2, v1

	// C[4], stored in v4[0], and v6[0]
	vsetivli  zero, 1, e64, m1, ta, ma
	vxor.vv   v4, v10, v14
	vxor.vv   v5, v18, v22
	vxor.vv   v6, v4, v26
	vxor.vv   v4, v6, v5
	// copying C[4] to v6[0]
	vmv.v	v6, v4

	// rot(C[0], 1) in v30[0], using v30 as temp
	vsll.vi v31, v2, 1
	vsrl.vi v30, v2, 63
	vor.vv v30, v30, v31

	// {C[1:3], C{4}} stored in v0v1 
	vsetivli  zero, 3, e64, m2, tu, ma
	vslidedown.vi v0, v2, 1 
	vsetivli  zero, 2, e64, m1, tu, ma
	vslideup.vi v1, v6, 1

	// extracting C[3] from v1[0] 
	// building D[4] in v30[0]
	vsetivli  zero, 1, e64, m1, tu, ma // could be skipped, reusing vl=2, m1 from before
	vxor.vv v30, v30, v1

	// {C[4], C[0:2]} stored in v4v5
	vsetivli  zero, 4, e64, m2, tu, ma
	vslideup.vi v4, v2, 1 

	// {D[0:3]} stored in v0v1
	vsetivli  zero, 4, e64, m2, tu, ma
	// rot({C[1:3], C[4]}, 1) in v0v1, using v28v29 as temp
	vsll.vi v28, v0, 1
	vsrl.vi v0, v0, 63
	vor.vv v0, v28, v0
	vxor.vv v0, v0, v4

	// end of theta step
	// A_<0:3>_0 = A_<0:3>_0 ^ D[0:3]
	vxror.vv v8, v8, v0
	// A_<0:3>_1 = A_<0:3>_1 ^ D[0:3]
	vxror.vv v12, v12, v0
	// A_<0:3>_2 = A_<0:3>_2 ^ D[0:3]
	vxror.vv v16, v16, v0
	// A_<0:3>_3 = A_<0:3>_3 ^ D[0:3]
	vxror.vv v20, v20, v0
	// A_<0:3>_4 = A_<0:3>_4 ^ D[0:3]
	vxror.vv v24, v24, v0

	vsetivli	zero, 1, e64, m1, ta, ma
	// A_<4>_0 = A_<4>_0 ^ D[4]
	vxor.vv v10, v10, v30
	// A_<4>_1 = A_<4>_1 ^ D[4]
	vxor.vv v14, v14, v30
	// A_<4>_2 = A_<4>_2 ^ D[4]
	vxor.vv v18, v18, v30
	// A_<4>_3 = A_<4>_3 ^ D[4]
	vxor.vv v22, v22, v30
	// A_<4>_4 = A_<4>_4 ^ D[4]
	vxor.vv v26, v26, v30

    // u64 T_0 = A_1_0;
    // u64 T_1 = A_0_2;
    // A_0_2 = ROL(T_0, 1);
    // u64 T_2 = A_2_1;
    // A_2_1 = ROL(T_1, 3);
    // u64 T_3 = A_1_2;
    // A_1_2 = ROL(T_2, 6);
    // u64 T_4 = A_2_3;
    // A_2_3 = ROL(T_3, 10);
    // u64 T_5 = A_3_3;
    // A_3_3 = ROL(T_4, 15);
    // u64 T_6 = A_3_0;
    // A_3_0 = ROL(T_5, 21);
    // u64 T_7 = A_0_1;
    // A_0_1 = ROL(T_6, 28);
    // u64 T_8 = A_1_3;
    // A_1_3 = ROL(T_7, 36);
    // u64 T_9 = A_3_1;
    // A_3_1 = ROL(T_8, 45);
    // u64 T_10 = A_1_4;
    // A_1_4 = ROL(T_9, 55);
    // u64 T_11 = A_4_4;
    // A_4_4 = ROL(T_10, 2);
    // u64 T_12 = A_4_0;
    // A_4_0 = ROL(T_11, 14);
    // u64 T_13 = A_0_3;
    // A_0_3 = ROL(T_12, 27);
    // u64 T_14 = A_3_4;
    // A_3_4 = ROL(T_13, 41);
    // u64 T_15 = A_4_3;
    // A_4_3 = ROL(T_14, 56);
    // u64 T_16 = A_3_2;
    // A_3_2 = ROL(T_15, 8);
    // u64 T_17 = A_2_2;
    // A_2_2 = ROL(T_16, 25);
    // u64 T_18 = A_2_0;
    // A_2_0 = ROL(T_17, 43);
    // u64 T_19 = A_0_4;
    // A_0_4 = ROL(T_18, 62);
    // u64 T_20 = A_4_2;
    // A_4_2 = ROL(T_19, 18);
    // u64 T_21 = A_2_4;
    // A_2_4 = ROL(T_20, 39);
    // u64 T_22 = A_4_1;
    // A_4_1 = ROL(T_21, 61);
    // u64 T_23 = A_1_1;
    // A_1_1 = ROL(T_22, 20);
    // u64 T_24 = A_1_0;
    // A_1_0 = ROL(T_23, 44);
    // u64 C_0_0 = A_0_0;
    // u64 C_0_1 = A_1_0;
    // u64 C_0_2 = A_2_0;
    // u64 C_0_3 = A_3_0;
    // u64 C_0_4 = A_4_0;
    // A_0_0 = C_0_0 ^ (~C_0_1 & C_0_2);
    // A_1_0 = C_0_1 ^ (~C_0_2 & C_0_3);
    // A_2_0 = C_0_2 ^ (~C_0_3 & C_0_4);
    // A_3_0 = C_0_3 ^ (~C_0_4 & C_0_0);
    // A_4_0 = C_0_4 ^ (~C_0_0 & C_0_1);
    // u64 C_1_0 = A_0_1;
    // u64 C_1_1 = A_1_1;
    // u64 C_1_2 = A_2_1;
    // u64 C_1_3 = A_3_1;
    // u64 C_1_4 = A_4_1;
    // A_0_1 = C_1_0 ^ (~C_1_1 & C_1_2);
    // A_1_1 = C_1_1 ^ (~C_1_2 & C_1_3);
    // A_2_1 = C_1_2 ^ (~C_1_3 & C_1_4);
    // A_3_1 = C_1_3 ^ (~C_1_4 & C_1_0);
    // A_4_1 = C_1_4 ^ (~C_1_0 & C_1_1);
    // u64 C_2_0 = A_0_2;
    // u64 C_2_1 = A_1_2;
    // u64 C_2_2 = A_2_2;
    // u64 C_2_3 = A_3_2;
    // u64 C_2_4 = A_4_2;
    // A_0_2 = C_2_0 ^ (~C_2_1 & C_2_2);
    // A_1_2 = C_2_1 ^ (~C_2_2 & C_2_3);
    // A_2_2 = C_2_2 ^ (~C_2_3 & C_2_4);
    // A_3_2 = C_2_3 ^ (~C_2_4 & C_2_0);
    // A_4_2 = C_2_4 ^ (~C_2_0 & C_2_1);
    // u64 C_3_0 = A_0_3;
    // u64 C_3_1 = A_1_3;
    // u64 C_3_2 = A_2_3;
    // u64 C_3_3 = A_3_3;
    // u64 C_3_4 = A_4_3;
    // A_0_3 = C_3_0 ^ (~C_3_1 & C_3_2);
    // A_1_3 = C_3_1 ^ (~C_3_2 & C_3_3);
    // A_2_3 = C_3_2 ^ (~C_3_3 & C_3_4);
    // A_3_3 = C_3_3 ^ (~C_3_4 & C_3_0);
    // A_4_3 = C_3_4 ^ (~C_3_0 & C_3_1);
    // u64 C_4_0 = A_0_4;
    // u64 C_4_1 = A_1_4;
    // u64 C_4_2 = A_2_4;
    // u64 C_4_3 = A_3_4;
    // u64 C_4_4 = A_4_4;
    // A_0_4 = C_4_0 ^ (~C_4_1 & C_4_2);
    // A_1_4 = C_4_1 ^ (~C_4_2 & C_4_3);
    // A_2_4 = C_4_2 ^ (~C_4_3 & C_4_4);
    // A_3_4 = C_4_3 ^ (~C_4_4 & C_4_0);
    // A_4_4 = C_4_4 ^ (~C_4_0 & C_4_1);
    // /*ι*/ // XL(0,0,RC[i]);
    // A_0_0 ^= RC[i];



	// computing C
	vxor.vv	v28, v8, v12
	vxor.vv	v0, v16, v20
	vxor.vv	v28, v28, v24
	vxor.vv	v28, v0, v28
	// computing D
	vsetivli	zero, 1, e64, m4, ta, ma
	vslidedown.vi	v0, v28, 4
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v0, v28, 1
	vslidedown.vi	v4, v28, 1
	vslideup.vi	v4, v28, 4
	// left rotation by 1
	vadd.vv	v28, v4, v4
	li	a2, 63
	vsrl.vx	v4, v4, a2
	vor.vv	v28, v28, v4
	// final D computation
	vxor.vv	v28, v28, v0

	// xoring D into A
	vxor.vv	v8, v8, v28
	vxor.vv	v12, v12, v28
	vxor.vv	v16, v16, v28
	vxor.vv	v20, v20, v28
	vxor.vv	v24, v24, v28

	// storing back state in memory
	vse64.v	v8, (a0)
	vse64.v	v12, (a5)
	vse64.v	v16, (t0)
	vse64.v	v20, (a7)
	vse64.v	v24, (a6)

	// rho and pi steps
.Lpcrel_hi0:
	auipc	a2, %pcrel_hi(.L__const.KeccakF1600_Round_vector.offset_AtoB)
	addi	a2, a2, %pcrel_lo(.Lpcrel_hi0)
	vsetivli	zero, 16, e64, m8, ta, ma
	vle16.v	v8, (a2)
.Lpcrel_hi1:
	auipc	a3, %pcrel_hi(.L__const.KeccakF1600_Round_vector.rotation_B)
	addi	a3, a3, %pcrel_lo(.Lpcrel_hi1)
	vle64.v	v16, (a3)
	vluxei16.v	v24, (a0), v8
	li	a4, 64
	vrsub.vx	v8, v16, a4
	vsll.vv	v16, v24, v16
	vsrl.vv	v8, v24, v8
	vor.vv	v8, v16, v8
	addi	a2, a2, 32
	vsetivli	zero, 9, e64, m8, ta, ma
	vle16.v	v16, (a2)
	addi	a2, a3, 128
	vle64.v	v24, (a2)
	vluxei16.v	v0, (a0), v16
	vrsub.vx	v16, v24, a4
	vsll.vv	v24, v0, v24
	vsrl.vv	v16, v0, v16
	vor.vv	v16, v24, v16
	vsetivli	zero, 16, e64, m8, ta, ma
	vse64.v	v8, (a0)
	addi	a2, a0, 128
	vsetivli	zero, 9, e64, m8, ta, ma
	vse64.v	v16, (a2)
	vsetivli	zero, 5, e64, m4, ta, ma
	vle64.v	v8, (a0)
	vsetivli	zero, 4, e64, m4, ta, ma
	vslidedown.vi	v12, v8, 1
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v12, v8, 4
	vnot.v	v12, v12
	vsetivli	zero, 3, e64, m4, ta, ma
	vslidedown.vi	v16, v8, 2
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v16, v8, 3
.Lpcrel_hi2:
	auipc	a2, %pcrel_hi(RC)
	addi	a2, a2, %pcrel_lo(.Lpcrel_hi2)
	sh3add.uw	t1, a1, a2
	ld	t1, 0(t1)
	vand.vv	v12, v12, v16
	vxor.vv	v8, v8, v12
	vsetivli	zero, 1, e64, m4, tu, ma
	vxor.vx	v8, v8, t1
	vsetivli	zero, 5, e64, m4, ta, ma
	vse64.v	v8, (a0)
	vle64.v	v8, (a5)
	vsetivli	zero, 4, e64, m4, ta, ma
	vslidedown.vi	v12, v8, 1
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v12, v8, 4
	vnot.v	v12, v12
	vsetivli	zero, 3, e64, m4, ta, ma
	vslidedown.vi	v16, v8, 2
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v16, v8, 3
	vand.vv	v12, v12, v16
	vxor.vv	v8, v8, v12
	vse64.v	v8, (a5)
	vle64.v	v8, (t0)
	vsetivli	zero, 4, e64, m4, ta, ma
	vslidedown.vi	v12, v8, 1
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v12, v8, 4
	vnot.v	v12, v12
	vsetivli	zero, 3, e64, m4, ta, ma
	vslidedown.vi	v16, v8, 2
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v16, v8, 3
	vand.vv	v12, v12, v16
	vxor.vv	v8, v8, v12
	vse64.v	v8, (t0)
	vle64.v	v8, (a7)
	vsetivli	zero, 4, e64, m4, ta, ma
	vslidedown.vi	v12, v8, 1
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v12, v8, 4
	vnot.v	v12, v12
	vsetivli	zero, 3, e64, m4, ta, ma
	vslidedown.vi	v16, v8, 2
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v16, v8, 3
	vand.vv	v12, v12, v16
	vxor.vv	v8, v8, v12
	vse64.v	v8, (a7)
	vle64.v	v8, (a6)
	vsetivli	zero, 4, e64, m4, ta, ma
	vslidedown.vi	v12, v8, 1
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v12, v8, 4
	vnot.v	v12, v12
	vsetivli	zero, 3, e64, m4, ta, ma
	vslidedown.vi	v16, v8, 2
	vsetivli	zero, 5, e64, m4, ta, ma
	vslideup.vi	v16, v8, 3
	vand.vv	v12, v12, v16
	vxor.vv	v8, v8, v12
	vse64.v	v8, (a6)

	addi a1, a1, 1
	addi t1, a1, -24
	bnez t1, KeccakF1600_Round_vector
	ret
.Lfunc_end0:
	.size	KeccakF1600_Round_vector, .Lfunc_end0-KeccakF1600_Round_vector
                                        # -- End function
                                        # -- End function
	.type	.L__const.KeccakF1600_Round_vector.offset_AtoB,@object # @__const.KeccakF1600_Round_vector.offset_AtoB
	.section	.rodata,"a",@progbits
	.p2align	1, 0x0
.L__const.KeccakF1600_Round_vector.offset_AtoB:
	.half	0                               # 0x0
	.half	48                              # 0x30
	.half	96                              # 0x60
	.half	144                             # 0x90
	.half	192                             # 0xc0
	.half	24                              # 0x18
	.half	72                              # 0x48
	.half	80                              # 0x50
	.half	128                             # 0x80
	.half	176                             # 0xb0
	.half	8                               # 0x8
	.half	56                              # 0x38
	.half	104                             # 0x68
	.half	152                             # 0x98
	.half	160                             # 0xa0
	.half	32                              # 0x20
	.half	40                              # 0x28
	.half	88                              # 0x58
	.half	136                             # 0x88
	.half	184                             # 0xb8
	.half	16                              # 0x10
	.half	64                              # 0x40
	.half	112                             # 0x70
	.half	120                             # 0x78
	.half	168                             # 0xa8
	.size	.L__const.KeccakF1600_Round_vector.offset_AtoB, 50

	.type	.L__const.KeccakF1600_Round_vector.rotation_B,@object # @__const.KeccakF1600_Round_vector.rotation_B
	.p2align	3, 0x0
.L__const.KeccakF1600_Round_vector.rotation_B:
	.quad	0                               # 0x0
	.quad	44                              # 0x2c
	.quad	43                              # 0x2b
	.quad	21                              # 0x15
	.quad	14                              # 0xe
	.quad	28                              # 0x1c
	.quad	20                              # 0x14
	.quad	3                               # 0x3
	.quad	45                              # 0x2d
	.quad	61                              # 0x3d
	.quad	1                               # 0x1
	.quad	6                               # 0x6
	.quad	25                              # 0x19
	.quad	8                               # 0x8
	.quad	18                              # 0x12
	.quad	27                              # 0x1b
	.quad	36                              # 0x24
	.quad	10                              # 0xa
	.quad	15                              # 0xf
	.quad	56                              # 0x38
	.quad	62                              # 0x3e
	.quad	55                              # 0x37
	.quad	39                              # 0x27
	.quad	41                              # 0x29
	.quad	2                               # 0x2
	.size	.L__const.KeccakF1600_Round_vector.rotation_B, 200

	.type	RC,@object                      # @RC
	.p2align	3, 0x0
RC:
	.quad	1                               # 0x1
	.quad	32898                           # 0x8082
	.quad	-9223372036854742902            # 0x800000000000808a
	.quad	-9223372034707259392            # 0x8000000080008000
	.quad	32907                           # 0x808b
	.quad	2147483649                      # 0x80000001
	.quad	-9223372034707259263            # 0x8000000080008081
	.quad	-9223372036854743031            # 0x8000000000008009
	.quad	138                             # 0x8a
	.quad	136                             # 0x88
	.quad	2147516425                      # 0x80008009
	.quad	2147483658                      # 0x8000000a
	.quad	2147516555                      # 0x8000808b
	.quad	-9223372036854775669            # 0x800000000000008b
	.quad	-9223372036854742903            # 0x8000000000008089
	.quad	-9223372036854743037            # 0x8000000000008003
	.quad	-9223372036854743038            # 0x8000000000008002
	.quad	-9223372036854775680            # 0x8000000000000080
	.quad	32778                           # 0x800a
	.quad	-9223372034707292150            # 0x800000008000000a
	.quad	-9223372034707259263            # 0x8000000080008081
	.quad	-9223372036854742912            # 0x8000000000008080
	.quad	2147483649                      # 0x80000001
	.quad	-9223372034707259384            # 0x8000000080008008
	.quad	0                               # 0x0
	.size	RC, 200

	.ident	"clang version 18.1.8 (https://github.com/nibrunie/llvm-project.git 41f4d0c021746f0b8770ece91f712fd0795d0c90)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
