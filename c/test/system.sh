#!/bin/sh
# System Tests - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
TMP=$(mktemp)
PASSED=0
FAILED=0

echo "SYSTEM TESTS"
echo "-------------------------------------------------------------"

# Test all-ops.ibcm
echo "a" > $TMP
test_output=$(./ibcmer.out test/all-ops.ibcm --check --silent < $TMP)
expect_output="19ed!"
if [ "$test_output" = "$expect_output" ]; then
	PASSED=$(($PASSED + 1))
else
	FAILED=$(($FAILED + 1))
	echo ""
	echo "  All operations code execution failed (all-ops.ibcm < a)"
	echo "    Expected: '$expect_output'"
	echo "    Received: '$test_output'"
fi

echo "0
ffff" > $TMP
test_output=$(./ibcmer.out test/all-ops.ibcm --check --silent < $TMP)
expect_output="05ed!"
if [ "$test_output" = "$expect_output" ]; then
	PASSED=$(($PASSED + 1))
else
	FAILED=$(($FAILED + 1))
	echo ""
	echo "  All operations code execution failed (all-ops.ibcm)"
	echo "    Expected: '$expect_output'"
	echo "    Received: '$test_output'"
fi

test_output=$(./ibcmer.out test/overflow.ibcm --check --silent)
expect_output="1d95ffa50180"
if [ "$test_output" = "$expect_output" ]; then
	PASSED=$(($PASSED + 1))
else
	FAILED=$(($FAILED + 1))
	echo ""
	echo "  All operations code execution failed (all-ops.ibcm)"
	echo "    Expected: '$expect_output'"
	echo "    Received: '$test_output'"
fi

if [ $FAILED -gt 0 ]; then
	echo ""
	echo "-------------------------------------------------------------"
fi




echo "Tests Passed: $PASSED"
if [ $FAILED -gt 0 ]; then
	printf "Tests Failed: \033[0;31m$FAILED\033[0m\n\n"
	exit 1
fi
