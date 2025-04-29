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
