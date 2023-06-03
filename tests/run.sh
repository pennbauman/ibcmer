#!/bin/sh
# Test all implementations - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
ROOT_DIR="$(dirname "$(dirname "$(realpath "$0")")")"
INPUT_DIR="$(realpath $ROOT_DIR/tests/input | sed "s/$(pwd | tr / .)/./")"
OUTPUT_DIR="$(realpath $ROOT_DIR/tests/output | sed "s/$(pwd | tr / .)/./")"
EXPECT_DIR="$(realpath $ROOT_DIR/tests/expect | sed "s/$(pwd | tr / .)/./")"

EXEC_PATHS="$ROOT_DIR/c/ibcmer.out
$ROOT_DIR/cpp/ibcmer.out
$ROOT_DIR/rust/target/release/ibcmer
$ROOT_DIR/go/ibcmer
$ROOT_DIR/python/ibcmer.py
$ROOT_DIR/shell/ibcmer.sh
"
CODE_PATHS="$ROOT_DIR/examples/hello-world.ibcm
$ROOT_DIR/examples/summation.ibcm
$ROOT_DIR/examples/multiply.ibcm
$ROOT_DIR/examples/array-summation.ibcm
$ROOT_DIR/examples/turing.ibcm
$ROOT_DIR/tests/all-ops.ibcm
$ROOT_DIR/tests/overflow.ibcm
"

runtest () {
	# setup local variables
	if [ -z "$3" ]; then
		test_id="$(basename "$2" | sed -E 's/\.[a-z]+//')"
	else
		test_id="$(basename "$2" | sed -E 's/\.[a-z]+//').$3"
	fi
	lang="$(echo "$1" | sed -E -e "s/$(echo $ROOT_DIR | tr / .)\///" -e "s/\/.*$//")"
	expect_file="$ROOT_DIR/tests/expect/$test_id.log"
	log_file="$OUTPUT_DIR/$lang/$test_id.log"
	diff_file="$OUTPUT_DIR/$lang/$test_id.diff"
	input_file="$INPUT_DIR/$test_id.txt"

	printf "\033[1;36m[%s] %s\033[0m\n" "$lang" "$(echo $test_id | sed 's/\./ - /')"
	mkdir -p "$OUTPUT_DIR/$lang"
	if [ -f "$input_file" ]; then
		"$1" "$2" > "$log_file" < "$input_file"
	else
		"$1" "$2" > "$log_file"
	fi
	diff "$log_file" "$expect_file" > "$diff_file"
	if [ $? -ne 0 ]; then
		echo "Test failed (output: $log_file, diff: $diff_file)"
		return 1
	fi
	return 0
}


rm -rf $OUTPUT_DIR
mkdir -p $OUTPUT_DIR

for bin in $EXEC_PATHS; do
	if [ ! -f "$bin" ]; then
		echo "Missing test executable '$bin'"
		exit 1
	fi

	for file in $CODE_PATHS; do
		if [ ! -f "$file" ]; then
			echo "Missing IBCM code file '$file'"
			exit 1
		fi
		test_name="$(basename "$file" | sed -E 's/\.[a-z]+//')"

		if [ -f "$EXPECT_DIR/$test_name.log" ]; then
			runtest "$bin" "$file"
		elif [ -f "$EXPECT_DIR/$test_name.1.log" ]; then
			i=1
			while [ -f "$INPUT_DIR/$test_name.$i.txt" ]; do
				runtest "$bin" "$file" "$i"
				if [ $? -ne 0 ]; then
					break
				fi
				i=$(($i + 1))
			done
		else
			echo "Expected output '$EXPECT_DIR/$test_name.log' not found"
		fi
	done
done
