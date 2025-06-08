#!/bin/sh
# IBCMer - Implementation Testing Script
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
ROOT_DIR="$(dirname "$(dirname "$(realpath "$0")")")"
cd $ROOT_DIR
# Directories
INPUT_DIR="./tests/input"
EXPECT_DIR="./tests/expect"
OUTPUT_DIR="./tests/output"
mkdir -p "$OUTPUT_DIR"
OUTPUT_DIR="$(mktemp -d "$OUTPUT_DIR/XXXX")"
# Results files
SUCCESS_FILE="$OUTPUT_DIR/successes"
FAILURE_FILE="$OUTPUT_DIR/failures"
echo 0 | tee "$SUCCESS_FILE" > "$FAILURE_FILE"

EXEC_LIST="c/ibcmer.out
cpp/ibcmer.out
rust/target/release/ibcmer
go/ibcmer
python/ibcmer.py
shell/ibcmer.sh
perl/ibcmer.pl
lua/ibcmer.lua
R/ibcmer.R
ruby/ibcmer.rb
zig/zig-out/bin/ibcmer
javascript/ibcmer.js
swift/.build/release/ibcmer
fish/ibcmer.fish
"
CODE_LIST="examples/hello-world.ibcm
examples/summation.ibcm
examples/multiply.ibcm
examples/array-summation.ibcm
examples/turing.ibcm
tests/all-ops.ibcm
tests/overflow.ibcm
tests/empty.ibcm
tests/halt-on-last.ibcm
tests/pc-overflow.ibcm
tests/codefile-invalid-hex.ibcm
tests/codefile-short-opcode.ibcm
tests/codefile-overflow.ibcm
"
HELP_TEXT="IBCMer Implementation Testing Script

  $0 [OPTIONS]

Options:
  --langs,-l [LANGS]   Language(s) to test as a comma seperated list
  --tests,-t [TESTS]   Test(s) to run as a comma seperated list
  --help,-h            Print this help menu
"


incfailures () {
	n="$(cat "$FAILURE_FILE")"
	echo $(($n + 1)) > "$FAILURE_FILE"
}

runtest () {
	# setup local variables
	test_id="$(basename "$2" | sed -E 's/\.[a-z]+//')"
	if [ ! -z "$3" ]; then
		test_id="$test_id.$3"
	fi
	lang="$(realpath -s --relative-to="$ROOT_DIR" "$1" | sed -E -e "s/\/.*$//")"
	expect_file="$EXPECT_DIR/$test_id.log"
	err_expect_file="$EXPECT_DIR/$test_id.err"
	log_file="$OUTPUT_DIR/$lang/$test_id.log"
	err_file="$OUTPUT_DIR/$lang/$test_id.err"
	diff_file="$OUTPUT_DIR/$lang/$test_id.diff"
	input_file="$INPUT_DIR/$test_id.txt"

	printf "\033[1;35m[%s] %s\033[0m\n" "$lang" "$(echo $test_id | sed 's/\./ - /')"
	mkdir -p "$OUTPUT_DIR/$lang"
	if [ -f "$input_file" ]; then
		"$1" "$2" > "$log_file" < "$input_file" 2> "$err_file"
	else
		"$1" "$2" > "$log_file" 2> "$err_file"
	fi
	sed -i "s,\x1B\[[0-9;]*[a-zA-Z],,g" "$OUTPUT_DIR/$lang/$test_id."*
	if [ -f "$err_expect_file" ]; then
		diff "$err_file" "$err_expect_file" > "$diff_file"
		if [ $? -ne 0 ]; then
			echo "Test failed error checks"
			echo "  errors: $err_file"
			echo "  diff: $diff_file"
			incfailures
			return 1
		fi
	else
		if [ "$(cat $err_file | wc -l)" -ne 0 ]; then
			echo "Test failed with unexpected errors"
			echo "  output: $log_file"
			echo "  errors: $err_file"
			incfailures
			return 1
		fi
	fi
	if [ -f "$expect_file" ]; then
		diff "$log_file" "$expect_file" > "$diff_file"
		if [ $? -ne 0 ]; then
			echo "Test failed"
			echo "  output: $log_file"
			echo "  diff: $diff_file"
			incfailures
			return 1
		fi
	else
		if [ "$(cat $log_file | wc -l)" -ne 0 ]; then
			echo "Test failed with unexpected output"
			echo "  output: $log_file"
			incfailures
			return 1
		fi
	fi
	# Increment successes
	n="$(cat "$SUCCESS_FILE")"
	echo $(($n + 1)) > "$SUCCESS_FILE"
	return 0
}

LANGS=""
TESTS=""
# Read command line options
if [ $# -gt 0 ]; then
	GET_VALUE=""
	for arg in "$@"; do
		if [ ! -z $GET_VALUE ]; then
			case $GET_VALUE in
				--langs|-l) LANGS="$arg" ;;
				--tests|-t) TESTS="$arg" ;;
				*) echo "UNREACHABLE CODE (\$GET_VALUE case)"; exit 1 ;;
			esac
			GET_VALUE=""
			continue
		fi
		#echo "arg: '$arg'"
		if [ ! -z $(echo $arg | grep -oE '^-*[hH](elp|)$') ]; then
			echo "$HELP_TEXT"
			exit 0
		fi

		case $arg in
			--langs|-l) GET_VALUE="$arg" ;;
			--tests|-t) GET_VALUE="$arg" ;;
			*) echo "Unknown option '$arg'"
				exit 1 ;;
		esac
	done
	# Check if value for option is still required
	case $GET_VALUE in
		"" ) ;;
		--langs|-l) echo "Missing language(s) for '$GET_VALUE' option" ; exit 1 ;;
		--tests|-t) echo "Missing test(s) for '$GET_VALUE' option" ; exit 1 ;;
		*) echo "UNREACHABLE CODE (\$GET_VALUE case)"; exit 1 ;;
	esac
fi

# Select languages to test
EXEC_PATHS=""
if [ -z "$LANGS" ]; then
	EXEC_PATHS="$EXEC_LIST"
else
	for l in $(echo "$LANGS" | sed 's/,/ /g'); do
		if [ ! -z "$(echo "$EXEC_LIST" | grep -oE "^$l/")" ]; then
			EXEC_PATHS="$EXEC_PATHS $(echo "$EXEC_LIST" | grep -E "^$l/")"
		else
			echo "Unknown language '$l'"
			exit 1
		fi
	done
fi

# Select tests to run
CODE_PATHS=""
if [ -z "$TESTS" ]; then
	CODE_PATHS="$CODE_LIST"
else
	for t in $(echo "$TESTS" | sed 's/,/ /g'); do
		if [ ! -z "$(echo "$CODE_LIST" | grep -oE "/$t(\.ibcm)?$")" ]; then
			CODE_PATHS="$CODE_PATHS $(echo "$CODE_LIST" | grep -E "/$t(\.ibcm)?$")"
		else
			echo "Unknown test '$t'"
			exit 1
		fi
	done
fi


# Iterate over languanges to test
for bin in $EXEC_PATHS; do
	if [ ! -f "$bin" ]; then
		echo "Missing test executable '$bin'"
		exit 1
	fi

	# Iterate over code files to test
	for file in $CODE_PATHS; do
		if [ ! -f "$file" ]; then
			echo "Missing IBCM code file '$file'"
			exit 1
		fi
		test_name="$(basename "$file" | sed -E 's/\.[a-z]+//')"

		if [ -f "$EXPECT_DIR/$test_name.log" ] || [ -f "$EXPECT_DIR/$test_name.err" ]; then
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

printf "\n\033[1;35mSuccesses: \033[0;32m%3d\033[0m\n" "$(cat "$SUCCESS_FILE")"
if [ "$(cat "$FAILURE_FILE")" -gt 0 ]; then
	printf "\033[1;35mFailures:  \033[1;31m%3d\033[0m\n" "$(cat "$FAILURE_FILE")"
	exit 1
fi
