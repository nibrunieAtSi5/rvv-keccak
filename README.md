# rvv-keccak
RISC-V Vector Implementation of Keccak hash function.

This implementation is based on C the implementation from https://github.com/XKCP/XKCP/tree/master/Standalone/CompactFIPS202/C.


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
- Baseline readable implementation in `Keccak-readable-and-compact.c`
- RVV based implementation in `keccak-vector-wrapper.c` (actual implementation is in `keccak-vector.c`)


Those implementations can be selected by setting the `KECCAK_SRC` variable to the corresponding source file when building and running the benchmark.
For example:

```
make sim_bench_keccak KECCAK_SRC=keccak-vector.c KECCAK_WRAPPER_SRC=keccak-vector-wrapper.c
```