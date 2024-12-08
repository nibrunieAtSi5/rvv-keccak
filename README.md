# rvv-keccak
RISC-V Vector Implementation of Keccak hash function.

This repository contains multiple experiments of vectorized implementation of the Keccak function (Keccak-F1600).
In particular it contains unrolled scalar C implementations and implementations using RISC-V Vector Extensions.

The implementations are based on C the implementation from https://github.com/XKCP/XKCP/tree/master/Standalone/CompactFIPS202/C.

A blog post described some of the techniques evaluated and summarized the results: https://fprox.substack.com/publish/posts/detail/140028226?referrer=github.


# Environment

## Building docker image

An environment to build the source contained in this repository can be created using the DockerFile `riscv-toolchain.Dockerfile`.

```
docker build  -t riscv:riscv-toolchain -f riscv-toolchain.Dockerfile .
```

## Running the docker image

```
docker run  -ti --mount type=bind,source="$(pwd)"/,target=/home/app/ riscv:riscv-toolchain
```

# Building and running the benchmark

This repository contains multiple implementations of the `Keccak` function.

- Baseline very compact implementation in `Keccak-more-compact.c`
- Optimized (unrolled) compact implementation in `Keccak-more-compact-opt.c`
- Optimized (unrolled with less memory accesses) compact implementation in `Keccak-more-compact-opt-in-regs.c`
- Baseline readable implementation in `Keccak-readable-and-compact.c`
- RVV based implementation in `keccak-vector-wrapper.c` (actual implementation is in `keccak-vector.c`)


Those implementations can be selected by setting the `KECCAK_SRC` and `KECCAK_WRAPPER_SRC` variables to the corresponding source file when building and running the benchmark.
For example:

```
make sim_bench_keccak KECCAK_SRC=keccak-vector.c KECCAK_WRAPPER_SRC=keccak-vector-wrapper.c CFLAGS="-O3"
```

## running without Zvbb support

By default the benchmark assumes that the target has support for the **Zvbb** (vector bit manipulation) extension.
The source code and the benchmark support disabling this extension:

```
make clean
make sim_bench_keccak CLANG_EXTRA_EXTS= SPIKE_EXTRA_EXTS= EXTRA_CFLAGS="-DNO_ZVBB_INTRINSICS"
```

In the code snippet below, use of scalar bit manipulation extensions has also been disabled, to enable it:

```
make clean
make sim_bench_keccak CLANG_EXTRA_EXTS=_zbb_zbc_zba SPIKE_EXTRA_EXTS=_zbb_zbc_zba EXTRA_CFLAGS="-DNO_ZVBB_INTRINSICS"
```