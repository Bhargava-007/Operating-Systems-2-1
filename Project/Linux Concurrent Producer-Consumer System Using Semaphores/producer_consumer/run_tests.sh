#!/usr/bin/env bash
set -e

DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN="$DIR/pc"

echo "=========================================================="
echo "    PRODUCER-CONSUMER CONCURRENCY & INVARIANT TEST SUITE   "
echo "=========================================================="

cd "$DIR"
make clean && make

run_test() {
    local test_name="$1"
    local buf="$2"
    local prod="$3"
    local cons="$4"
    local items="$5"
    local session="$6"

    echo ""
    echo ">>> Running Test: $test_name (Buffer: $buf, Prod: $prod, Cons: $cons, Items/P: $items)"

    # Run with timeout to catch any deadlock
    timeout 15s "$BIN" "$buf" "$prod" "$cons" "$items" "$session"
    local status=$?

    if [ $status -eq 0 ]; then
        echo "--> [PASS] $test_name exited cleanly (code 0)"
    elif [ $status -eq 124 ]; then
        echo "--> [FAIL] $test_name DEADLOCK DETECTED (Timed out after 15s)"
        exit 1
    else
        echo "--> [FAIL] $test_name failed with status $status"
        exit 1
    fi
}

# 1. Boundary: Buffer size 1 (strictly serialized producer-consumer handoff)
run_test "Boundary Buffer Size 1" 1 1 1 5 "test_boundary_buf1"

# 2. Boundary: Large Buffer size 20
run_test "Large Buffer Size 20" 20 2 2 10 "test_buf20"

# 3. Asymmetric High Producer Contention (6 Producers, 1 Consumer)
run_test "High Producer Contention (6P:1C)" 5 6 1 4 "test_high_prod"

# 4. Asymmetric High Consumer Contention (1 Producer, 6 Consumers)
run_test "High Consumer Contention (1P:6C)" 5 1 6 12 "test_high_cons"

# 5. Stress Test: High concurrency with repeated iterations
run_test "High Concurrency Stress Test (5P:5C, Buffer 8, 20 items)" 8 5 5 20 "test_stress"

# 6. Minimal Boundary Test (1P:1C, Buffer 2, 2 items)
run_test "Minimal Quick (1P:1C, Buffer 2)" 2 1 1 2 "test_minimal"

echo ""
echo "=========================================================="
echo "    ALL 6 TEST SCENARIOS PASSED WITH ZERO DEADLOCKS!      "
echo "=========================================================="
