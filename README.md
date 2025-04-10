# stackadler32

A Go implementation of the Adler-32 checksum algorithm with the core Update function implemented in C, optimized with Arm NEON instructions for superior performance.

## Overview

This package provides an Adler-32 checksum implementation where:
- The Go code handles the API and interface
- The performance-critical Update function is implemented in C and compiled as a shared library
- Arm NEON instructions are used for significant performance improvements

## Performance Results

The optimized implementation achieves consistent performance advantages over the standard library. The following benchmarks were performed on Arm Neoverse N1 processors:

| Data Size | NEON Implementation | Standard Library | Improvement  |
|-----------|---------------------|------------------|--------------|
| 10KB      | 5,479 ns/op         | 6,733 ns/op      | 18.6% faster |
| 1MB       | 503,957 ns/op       | 674,823 ns/op    | 25.3% faster |
| 10MB      | 4,997,849 ns/op     | 6,731,980 ns/op  | 25.8% faster |

## Building

To build the shared library:

```bash
make
```

This will compile the C code into a shared library (`libadler32.so`).

You can also use the following make targets:

```bash
make help      # Show available make targets and their descriptions
make gcc       # Build using GCC (default)
make clang     # Build using Clang
make test      # Run tests
make bench     # Run benchmarks with extended duration (5s)
make test-all  # Run benchmarks with both GCC and Clang for comparison
make clean     # Remove compiled files
```

You can also use the following utility scripts:

```bash
./run_small_bench.sh  # Run 5 iterations of small data benchmarks for more stable measurements
```

This script cleans and rebuilds the project, then runs multiple iterations of the small data benchmarks to provide more consistent performance measurements. It's useful for getting reliable numbers for the most performance-critical small data case.

## Usage

```go
package main

import (
    "fmt"
    "stackadler32"
)

func main() {
    data := []byte("hello world")
    checksum := stackadler32.Checksum(data)
    fmt.Printf("Adler-32 checksum: 0x%x\n", checksum)
    
    // Or use the digest API for incremental updates
    digest := stackadler32.NewDigest()
    digest = digest.Update([]byte("hello "))
    digest = digest.Update([]byte("world"))
    checksum = digest.Sum32()
    fmt.Printf("Adler-32 checksum: 0x%x\n", checksum)
}
```

## Optimization Techniques

The implementation uses several key optimization techniques:

1. **Block Processing**: Process data in blocks of 5552 bytes to minimize expensive modulo operations
2. **NEON Loading**: Use `vld1q_u8` to load 16 bytes at a time
3. **Byte Extraction**: Extract bytes using `vgetq_lane_u8` with constant indices
4. **Delayed Modulo**: Apply modulo operations only after processing each block
5. **Compiler Optimizations**: Use carefully selected compiler flags for maximum performance

## Implementation Details

The optimized implementation:

1. Handles small buffers (<16 bytes) with a standard scalar implementation
2. Divides large buffers into blocks of 5552 bytes (maximum size before overflow)
3. Processes each block in chunks of 16 bytes using NEON instructions
4. Applies modulo operations only after processing each block
5. Handles remaining bytes with the same approach

## Compiler Optimizations

After extensive experimentation, the following compiler flags were found to provide the best performance:

```
-march=native -O3 -ftree-vectorize -ffast-math -flto
```

These flags enable:
- CPU-specific optimizations with `-march=native`
- Aggressive optimization with `-O3` and `-ftree-vectorize`
- Mathematical optimizations with `-ffast-math`
- Link-time optimization with `-flto`

Interestingly, adding more optimization flags like `-funroll-loops`, `-fomit-frame-pointer`, and `-falign-functions=64` did not improve performance and in some cases made it worse.

## Compiler Selection

After comparing GCC and Clang with extended benchmarks (5s per test), we found that GCC produces more efficient code:

- Small data (10KB): GCC is ~11.6% faster than Clang
- Medium data (1MB): Both compilers perform similarly
- Large data (10MB): Both compilers perform similarly

Based on these results, we've standardized on using GCC for compilation.

## Challenges Overcome

1. **Lane Index Requirements**: Arm NEON requires constant immediate values for lane indices
2. **Overflow Prevention**: Careful block sizing to prevent integer overflow
3. **Correctness vs. Speed**: Balancing optimization with algorithm correctness
4. **Memory Access Patterns**: Ensuring efficient memory access with NEON instructions
5. **Compiler Flag Selection**: Finding the optimal set of compiler flags through experimentation

## Future Improvements

Potential areas for further optimization:

1. Assembly-level optimizations for critical loops
2. Exploring different block sizes for specific architectures
3. Implementing platform-specific optimizations
4. Reducing memory operations with register reuse
5. Profile-guided optimization (PGO) for workload-specific tuning

## Requirements

- Go 1.23.1 or later
- GCC compiler for building the C shared library

## License

See the LICENSE file for details.
