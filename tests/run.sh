#!/bin/sh
# Test all implementations - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
TMP_DIR="$(mktemp -d /tmp/ibcmer-XXXXXX)"
ROOT_DIR="$(dirname "$(dirname "$(readlink -f "$0")")")"
INPUT_DIR="$ROOT_DIR/tests/input"

EXEC_PATHS="$ROOT_DIR/c/ibcmer.out
$ROOT_DIR/cpp/ibcmer.out
$ROOT_DIR/rust/target/release/ibcmer
$ROOT_DIR/go/ibcmer
$ROOT_DIR/python/ibcmer.py
"
CODE_PATHS="$ROOT_DIR/examples/hello-world.ibcm
$ROOT_DIR/examples/summation.ibcm
$ROOT_DIR/examples/multiply.ibcm
$ROOT_DIR/examples/array-summation.ibcm
$ROOT_DIR/examples/turing.ibcm
$ROOT_DIR/tests/all-ops.ibcm
$ROOT_DIR/tests/overflow.ibcm
"

for bin in $EXEC_PATHS; do
	if [ ! -f "$bin" ]; then
		echo "Missing '$bin'"
		rm -rf $TMP_DIR
		exit 1
	fi
	lang="$(echo "$bin" | sed -E -e "s/$(echo "$ROOT_DIR" | sed 's/\//\\\//g')\///" -e "s/\/.*$//")"

	for file in $CODE_PATHS; do
		if [ ! -f "$file" ]; then
			echo "Missing '$file'"
			rm -rf $TMP_DIR
			exit 1
		fi
		test_name="$(basename "$file" | sed -E 's/\.[a-z]+//')"

		if [ -z "$(find "$INPUT_DIR" -name "$test_name*")" ]; then
			printf "\033[1;36m%s\033[0m\n" "[$lang] $test_name"
			expect_file="$ROOT_DIR/tests/expect/$test_name.log"
			tmp_file="$TMP_DIR/$test_name.$lang.log"

			$bin $file > $tmp_file
			diff $tmp_file $expect_file
		else
			i=1
			while [ -e "$INPUT_DIR/$test_name.$i.txt" ]; do
				printf "\033[1;36m%s\033[0m\n" "[$lang] $test_name - $i"
				expect_file="$ROOT_DIR/tests/expect/$test_name.$i.log"
				tmp_file="$TMP_DIR/$test_name.$lang.$i.log"

				$bin $file > $tmp_file < "$INPUT_DIR/$test_name.$i.txt"
				diff $tmp_file $expect_file
				i=$(($i + 1))
			done
		fi
	done
done

rm -rf $TMP_DIR
