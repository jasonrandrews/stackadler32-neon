#!/bin/bash

# Script to run multiple iterations of the small data benchmark
# to get a more stable measurement

echo "Running multiple benchmark iterations for small data..."
echo "This will take a few minutes..."

# Clean and build first
make clean && make

# Run 5 iterations of just the small data benchmarks
for i in {1..5}; do
  echo "Iteration $i:"
  go test -bench="BenchmarkThis$|BenchmarkStdLib$" -benchmem ./...
  echo ""
  sleep 2 # Small pause between runs
done

echo "Benchmark complete."
