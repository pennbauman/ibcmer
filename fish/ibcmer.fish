#!/bin/fish
# IBCMer - Fish Shell
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
set -gx MEM_SIZE 4096


# Check arguments, expect one code file
if test (count $argv) -eq 0
	printf "\033[31mError:\033[0m %s\n" "A code file must be provided" >&2
	exit 1
end
if test ! -f $argv[1]
	printf "\033[31mError:\033[0m %s\n" "Code file '$argv[1]' not found" >&2
	exit 1
end


# Read in code file
set -f i 1
cat $argv[1] | while read -L l
	if test $i -gt $MEM_SIZE
		printf "\033[31mError:\033[0m %s\n" "Code file overflows memory ($MEM_SIZE lines max)" >&2
		exit 1
	end
	set -f opcode (echo "$l" | grep -oE '^[0-9a-fA-F]{4}')
	if test -z $opcode
		set -f j 0
		while test ! -z "$(echo "$l" | grep -oE "^.{$j}[0-9a-fA-F]")"
			set -f j (math $j + 1)
		end
		printf "\033[31mError:\033[0m %s\n" "'$argv[1]:$(math $i):$(math $j + 1)' Invalid opcode hexadecimal" >&2
		echo >&2
		echo "    $l" >&2
		printf "    %$(math $j + 1)s\n" "^" >&2
		exit 1
	end
	set -gx MEM[$i] (string lower $opcode)
	set -f i (math $i + 1)
end
# Fill the rest of memory with zeros
while test $i -le $MEM_SIZE
	set -gx MEM[$i] 0000
	set -f i (math $i + 1)
end


# Initialize IBCM registers
set -gx PC 000
set -gx ACC 0000

# Run machine
while true
	set -f opcode $MEM[(math 0x$PC + 1)]
	set -f address (string sub -s 2 $opcode)
	printf "[%03x]%04x  " 0x$PC 0x$opcode

	switch (string sub -e 1 $opcode)
		case "0" # halt
			echo "halt"
			exit 0
		case "1" # i/o
			printf "i/o   (ACC)%04x\n" 0x$ACC
			switch (string sub -e 1 $address)
				case "0"
					printf "Input hex:  "
					read -L in
					if test -z (echo $in | grep -oE '^[0-9a-fA-F]{1,4}$')
						echo "  Invalid hex input '$in'" >&2
						exit 1
					else
						set -gx ACC (printf "%04x" "0x$in")
					end
				case "4"
					printf "Input char: "
					read -L in
					if test (string length $in) -ne 1
						echo "  Invalid char input" >&2
						exit 1
					else
						set -gx ACC (printf "%04x" "'$in")
					end
				case "8"
					printf "Output hex:  %04x\n" 0x$ACC
				case "c"
					set -f out (printf '%03o' 0x$(string sub -s 3 $ACC))
					printf "Output char: \\$out\n" ;;
				case "*"
					printf "\033[31mError:\033[0m Invalid I/O sub-opcode '$(string sub -e 1 $address)'\n" >&2
					exit 1
			end
		case "2" # shift
			set -f distance (math 0x$(string sub -s -1 $opcode))
			set -f bits (string pad -c 0 -w 16 (echo "ibase=16; obase=2; $(string upper $ACC)" | bc))
			switch (string sub -e 1 $address)
				case "0"
					set -f res (string pad -r -c 0 -w 16 (string sub -s (math $distance + 1) $bits))
					set -f arrow "<<"
				case "4"
					set -f res (string pad -c 0 -w 16 (string sub -e -$distance $bits))
					set -f arrow ">>"
				case "8"
					set -f res (string sub -s (math $distance + 1) $bits)(string sub -e $distance  $bits)
					set -f arrow "<="
				case "c"
					set -f res (string sub -s -$distance $bits)$(string sub -e (math 16 - $distance) $bits)
					set -f arrow "=>"
				case "*"
					printf "\033[31mError:\033[0m Invalid shift sub-opcode '$(string sub -e 1 $address)'\n" >&2
					exit 1
			end
			set -f res (string pad -c 0 -w 4 (string lower (echo "obase=16; ibase=2; $res" | bc)))
			printf "shift (ACC)%s = (ACC)%s %s %s\n" $res $ACC $arrow (string sub -s -1 $opcode)
			set -gx ACC $res
		case "3" # load
			set -f val $MEM[(math 0x$address + 1)]
			printf "load  (ACC)%s = [%s]%s\n" $val $address $val
			set -gx ACC $val
		case "4" # store
			set -gx MEM[(math 0x$address + 1)] $ACC
			printf "store [%s]%s = (ACC)%s\n" $address $ACC $ACC
		case "5" # add
			set -f val $MEM[(math 0x$address + 1)]
			set -f res (string pad -c 0 -w 4 (string sub -s -4 (string sub -s 3 (math --base=hex 0x$ACC + 0x$val))))
			printf "add   (ACC)%s = (ACC)%s + [%s]%s\n" $res $ACC $address $val
			set -gx ACC $res
		case "6" # sub
			set -f val $MEM[(math 0x$address + 1)]
			set -f res (math --base=hex 0x$ACC - 0x$val)
			if test (math $res) -lt 0
				set -f res (string sub -s 3 (math --base=hex $res + 0x10000))
			else
				set -f res (string sub -s 3 $res)
			end
			set -f res (string pad -c 0 -w 4 $res)
			printf "sub   (ACC)%s = (ACC)%s - [%s]%s\n" $res $ACC $address $val
			set -gx ACC $res
		case "7" # and
			set -f val $MEM[(math 0x$address + 1)]
			set -f res (printf '%04x' (math bitand 0x$ACC, 0x$val))
			printf "and   (ACC)%s = (ACC)%s & [%s]%s\n" $res $ACC $address $val
			set -gx ACC $res
		case "8" # or
			set -f val $MEM[(math 0x$address + 1)]
			set -f res (printf '%04x' (math bitor 0x$ACC, 0x$val))
			printf "or    (ACC)%s = (ACC)%s | [%s]%s\n" $res $ACC $address $val
			set -gx ACC $res
		case "9" # xor
			set -f val $MEM[(math 0x$address + 1)]
			set -f res (printf '%04x' (math bitxor 0x$ACC, 0x$val))
			printf "xor   (ACC)%s = (ACC)%s ^ [%s]%s\n" $res $ACC $address $val
			set -gx ACC $res
		case "a" # not
			set -f res (printf '%04x' (math bitxor 0x$ACC, 0xffff))
			printf "not   (ACC)%s = ! (ACC)%s\n" $res $ACC
			set -gx ACC $res
		case "b" # nop
			echo "nop"
		case "c" # jmp
			printf "jmp   [%s]\n" $address
			set -gx PC $address
			continue
		case "d" # jmpe
			if test $ACC = 0000
				printf "jmpe  [%s]\n" $address
				set -gx PC $address
				continue
			else
				printf "jmpe  (ACC)%s\n" $ACC
			end
		case "e" # jmpl
			if test -z (echo $ACC | grep -oE '^[89abcdef]')
				printf "jmpl  (ACC)%s\n" $ACC
			else
				printf "jmpl  [%s]\n" $address
				set -gx PC $address
				continue
			end
		case "f" # jmpl
			set -gx ACC (string pad -c 0 -w 4 (string sub -s 3 (math --base=hex 0x$PC + 1)))
			set -gx PC $address
			printf "brl   [%s]  (ACC)%s\n" $address $ACC
			continue
		case "*"
			printf "\033[31mError:\033[0m Invalid opcode '$opcode'\n" >&2
			exit 1
	end

	# Increment program counter
	set -gx PC (string sub -s 3 (math --base=hex 0x$PC + 1))
	if test (math 0x$PC) -ge $MEM_SIZE
		printf "\033[31mError:\033[0m Memory overflow (PC = 0x$PC)\n" >&2
		exit 1
	end
end
