#!/bin/sh
# IBCMer - POSIX Shell
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
MEM_SIZE=4096

# Check arguments, expect one code file
if [ $# -eq 0 ]; then
	echo "Missing code file"
	exit 1
fi
if [ ! -f "$1" ]; then
	if [ -e "$1" ]; then
		echo "Code file '$1' not a file"
	else
		echo "Code file '$1' does not exist"
	fi
	exit 1
fi

# Read in code file
MEM=$(mktemp /tmp/ibcmer-XXXXXXXXXXXX)
i=0
while read -r l; do
	opcode="$(echo "$l" | grep -oE '^[0-9a-fA-F]{4}')"
	if [ -z "$opcode" ]; then
		echo "Invalid opcode on line $i"
		rm -f $MEM
		exit 1
	fi
	echo "$opcode" | tr 'a-f' 'A-F' >> $MEM
	i=$(($i + 1))
done < "$1"
# Fill the rest of memory with zeros
while [ $i -le $MEM_SIZE ]; do
	echo "0000" >> $MEM
	i=$(($i + 1))
done

# Initialize IBCM registers
PC=0
ACC=0000

while true; do
	opcode=$(sed -n "$(($PC + 1))p" $MEM)
	address=$(echo $opcode | grep -oE '...$')
	printf "[%03x]%04x  " $PC 0x$opcode

	case "$(echo $opcode | grep -oE '^.')" in
		# halt
		0) echo "halt"
			rm -f $MEM
			exit 0 ;;
		# i/o
		1) printf "i/o   (ACC)%04x\n" 0x$ACC
			subopcode=$(echo $address | grep -oE '^.')
			case $subopcode in
				0) printf "Input hex:  "
					read in
					if [ -z "$(echo "$in" | grep -oE '^[0-9a-fA-F]{1,4}$')" ]; then
						echo "Invalid hex input '$in'"
						rm -f $MEM
						exit 1
					else
						ACC=$(printf "%04X" "0x$in")
					fi ;;
				4) printf "Input char: "
					read in
					if [ -z "$(echo "$in" | grep -oE '^.$')" ]; then
						echo "Invalid char input"
						rm -f $MEM
						exit 1
					else
						ACC=$(printf "%04X" "'$in")
					fi ;;
				8) printf "Output hex:  %04x\n" 0x$ACC ;;
				c|C) out=$(printf '%03o' "0x$(echo $ACC | grep -oE '..$')")
					printf "Output char: \\$out\n" ;;
				*) echo "Unknown i/o sub-opcode '$subopcode'"
					rm -f $MEM
					exit 1 ;;
			esac ;;
		# shift
		2) subopcode=$(echo $address | grep -oE '^.')
			distance=$(echo $address | grep -oE '.$')
			case $subopcode in
				0) res=$(printf '%04X' $((0x$ACC << 0x$distance)) | grep -oE '.{1,4}$')
					arrow="<<" ;;
				4) res=$(printf '%04X' $((0x$ACC >> 0x$distance)))
					arrow=">>" ;;
				8) a=$((0x$ACC << 0x$distance))
					b=$((0x$ACC >> (16-0x$distance)))
					res=$(printf '%04X' $(($a | $b)) | grep -oE '.{1,4}$')
					arrow="<=" ;;
				c|C) a=$((0x$ACC >> 0x$distance))
					b=$((0x$ACC << (16-0x$distance)))
					res=$(printf '%04X' $(($a | $b)) | grep -oE '.{1,4}$')
					arrow="=>" ;;
				*) echo "Unknown shift sub-opcode '$subopcode'"
					rm -f $MEM
					exit 1 ;;
			esac
			printf "shift (ACC)%04x = (ACC)%04x %s %x\n" 0x$res 0x$ACC $arrow 0x$distance
			ACC=$res ;;
		# load
		3) val=$(sed -n "$((0x$address + 1))p" $MEM)
			printf "load  (ACC)%04x = [%03x]%04x\n" 0x$val 0x$address 0x$val
			if [ -z "$(echo "$val" | grep -oE '^[0-9a-fA-F]{4}$')" ]; then
				echo "Invalid load '$val' at 0x$address"
				rm -f $MEM
				exit 1
			fi
			ACC=$val ;;
		# store
		4) sed -i "$((0x$address + 1))s/.*/$ACC/" $MEM
			printf "store [%03x]%04x = (ACC)%04x\n" 0x$address 0x$ACC 0x$ACC ;;
		# add
		5) val=$(sed -n "$((0x$address + 1))p" $MEM)
			res=$(printf '%04X' $((0x$ACC + 0x$val)) | grep -oE '.{1,4}$')
			printf "add   (ACC)%04x = (ACC)%04x + [%03x]%04x\n" 0x$res 0x$ACC 0x$address 0x$val
			ACC=$res ;;
		# sub
		6) val=$(sed -n "$((0x$address + 1))p" $MEM)
			res=$((0x$ACC - 0x$val))
			if [ "$res" -lt 0 ]; then
				res=$(printf '%04X' $((0x10000 + $res)))
			else
				res=$(printf '%04X' $res)
			fi
			printf "sub   (ACC)%04x = (ACC)%04x - [%03x]%04x\n" 0x$res 0x$ACC 0x$address 0x$val
			ACC=$res ;;
		# and
		7) val=$(sed -n "$((0x$address + 1))p" $MEM)
			res=$(printf '%04X' $((0x$ACC & 0x$val)))
			printf "and   (ACC)%04x = (ACC)%04x & [%03x]%04x\n" 0x$res 0x$ACC 0x$address 0x$val
			ACC=$res ;;
		# or
		8) val=$(sed -n "$((0x$address + 1))p" $MEM)
			res=$(printf '%04X' $((0x$ACC | 0x$val)))
			printf "or    (ACC)%04x = (ACC)%04x | [%03x]%04x\n" 0x$res 0x$ACC 0x$address 0x$val
			ACC=$res ;;
		# xor
		9) val=$(sed -n "$((0x$address + 1))p" $MEM)
			res=$(printf '%04X' $((0x$ACC ^ 0x$val)))
			printf "xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x\n" 0x$res 0x$ACC 0x$address 0x$val
			ACC=$res ;;
		# not
		a|A) val=$(sed -n "$((0x$address + 1))p" $MEM)
			res=$(printf '%04X' $((~ 0x$ACC)) | grep -oE '.{1,4}$')
			printf "not   (ACC)%04x = ! (ACC)%04x\n" 0x$res 0x$ACC
			ACC=$res ;;
		# nop
		b|B) echo "nop" ;;
		# jmp
		c|C) printf "jmp   [%03x]\n" 0x$address
			PC=$((0x$address - 1)) ;;
		# jmpe
		d|D) if [ "$ACC" = "0000" ]; then
				printf "jmpe  [%03x]\n" 0x$address
				PC=$((0x$address - 1))
			else
				printf "jmpe  (ACC)%04x\n" 0x$ACC
			fi ;;
		# jmpl
		e|E) if [ -z "$(echo "$ACC" | grep -oE '^[89ABCDEF]')" ]; then
				printf "jmpl  (ACC)%04x\n" 0x$ACC
			else
				printf "jmpl  [%03x]\n" 0x$address
				PC=$((0x$address - 1))
			fi ;;
		# brl
		f|F) ACC=$(printf "%04x" $((PC + 1)) | tr 'a-f' 'A-F')
			PC=$((0x$address - 1))
			printf "brl   [%03x]  (ACC)%04x\n" 0x$address 0x$ACC ;;
		*) echo "Unknown opcode '$opcode'"
			rm -f $MEM
			exit 1 ;;
	esac
	PC=$(($PC + 1))
	if [ $PC -ge $MEM_SIZE ]; then
		echo "Memory overflow (PC = $PC)"
		rm -f $MEM
		exit 1
	fi
done
