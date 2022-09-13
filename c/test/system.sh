#!/bin/bash
# System Tests - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author:
#     Penn Bauman (pennbauman@protonmail.com)
PASSED=0
FAILED=0

echo -e "\033[0;32mSYSTEM TESTS\033[0m"
echo "-------------------------------------------------------------"

# Test all-ops.ibcm
test_output=$(./ibcmer test/all-ops.ibcm --check --silent <<< a)
if [[ "$test_output" = "19ed!" ]]; then
	PASSED=$(($PASSED + 1))
else
	FAILED=$(($FAILED + 1))
	echo ""
	echo "  All operations code execution failed (all-ops.ibcm < a)"
	echo "    Expected: '19ed!'"
	echo "    Received: '$test_output'"
fi

test_output=$(./ibcmer test/all-ops.ibcm --check --silent <<< "0
ffff")
expect_output="05ed!"
if [[ "$test_output" == $expect_output ]]; then
	PASSED=$(($PASSED + 1))
else
	FAILED=$(($FAILED + 1))
	echo ""
	echo "  All operations code execution failed (all-ops.ibcm)"
	echo "    Expected: '$expect_output'"
	echo "    Received: '$test_output'"
fi

if (($FAILED > 0)); then
	echo ""
	echo "-------------------------------------------------------------"
fi




echo "Tests Passed: $PASSED"
if (($FAILED > 0)); then
	echo -e "Tests Failed: \033[0;31m$FAILED\033[0m"
	exit 1
fi
