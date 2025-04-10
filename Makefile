.PHONY: all clean standard neon test gcc clang test-all bench help

# Compiler selection
CC_GCC = gcc
CC_CLANG = clang

# Default compiler - GCC provides better performance
CC = $(CC_GCC)

# Compiler flags for optimization
CFLAGS = -march=native -O3 -ftree-vectorize -ffast-math -flto

# Default target builds with GCC
all: build

# Help target to explain available options
help:
	@echo "Available make targets:"
	@echo "  all, build  - Build the shared library using the default compiler (GCC)"
	@echo "  gcc         - Build the shared library using GCC"
	@echo "  clang       - Build the shared library using Clang"
	@echo "  clean       - Remove the compiled shared library"
	@echo "  test        - Run Go tests"
	@echo "  bench       - Run benchmarks with extended duration (5s)"
	@echo "  test-all    - Run benchmarks with both GCC and Clang for comparison"

# Build with GCC (default)
gcc: clean
	$(CC_GCC) -shared -fPIC -o libadler32.so $(CFLAGS) adler32.c

# Build with Clang
clang: clean
	$(CC_CLANG) -shared -fPIC -o libadler32.so $(CFLAGS) adler32.c

# Standard build target
build: clean
	$(CC) -shared -fPIC -o libadler32.so $(CFLAGS) adler32.c

clean:
	rm -f libadler32.so

test: 
	go test -v ./...

# Run benchmarks with longer duration for more stable results
bench:
	go test -bench=. -benchtime=5s -benchmem ./...

# Run tests with both compilers for comparison
test-all:
	@echo "Building with GCC..."
	@make gcc
	@echo "Testing with GCC build..."
	go test -bench=. -benchtime=5s -benchmem ./...
	@echo ""
	@echo "Building with Clang..."
	@make clang
	@echo "Testing with Clang build..."
	go test -bench=. -benchtime=5s -benchmem ./...
