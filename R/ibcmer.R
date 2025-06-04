#!/usr/bin/env Rscript
# IBCMer - R
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
library("bitops")
MEM_SIZE <- 4096


MEMORY <- array(0, MEM_SIZE)
ACC <- 0
PC <- 1


# Read code file
input_file <- commandArgs(trailingOnly=TRUE)
i <- 1
for (l in readLines(input_file)) {
	MEMORY[i] <- strtoi(substr(l, 0, 4), 16)
	i <- i + 1
}
stdin <- file("stdin", "r")


# Execute code
while (TRUE) {
	opcode <- bitShiftR(MEMORY[PC], 12)
	address <- bitwAnd(MEMORY[PC], 0x0fff)
	cat(sprintf("[%03x]%04x  ", PC - 1, MEMORY[PC]))

	# halt
	if (opcode == 0x0) {
		cat("halt\n")
		break
	# i/o
	} else if (opcode == 0x1) {
		subopcode = bitShiftR(address, 8)
		cat(sprintf("i/o   (ACC)%04x\n", ACC))
		if (subopcode == 0x0) {
			cat("Input hex:  ")
			text <- readLines(stdin, n=1)
			if (nchar(text) > 4) {
				stop(sprintf("Invalid hex input '%s'", text))
			}
			ACC <- strtoi(text, 16)
		} else if (subopcode == 0x4) {
			cat("Input char: ")
			text <- readLines(stdin, n=1)
			if (nchar(text) != 1) {
				stop("A single character must be entered")
			}
			ACC <- utf8ToInt(text)
		} else if (subopcode == 0x8) {
			cat(sprintf("Output hex:  %04x\n", ACC))
		} else if (subopcode == 0xc) {
			cat(sprintf("Output char: %s\n", intToUtf8(ACC)))
		} else {
			write(sprintf("Unknown i/o sub-opcode '%x'", subopcode), stderr())
			quit(status = 1)
		}
	# shift
	} else if (opcode == 0x2) {
		subopcode = bitShiftR(address, 8)
		distance <- bitwAnd(address, 0xf)
		if (subopcode == 0x0) {
			res = bitwAnd(bitShiftL(ACC, distance), 0xffff)
			arrow <- "<<"
		} else if (subopcode == 0x4) {
			res = bitwAnd(bitShiftR(ACC, distance), 0xffff)
			arrow <- ">>"
		} else if (subopcode == 0x8) {
			res = bitwAnd(bitwOr(bitShiftL(ACC, distance), bitShiftR(ACC, (16 - distance))), 0xffff)
			arrow <- "<="
		} else if (subopcode == 0xc) {
			res = bitwAnd(bitwOr(bitShiftR(ACC, distance), bitShiftL(ACC, (16 - distance))), 0xffff)
			arrow <- "=>"
		} else {
			write(sprintf("Unknown shift sub-opcode '%x'", subopcode), stderr())
			quit(status = 1)
		}
		cat(sprintf("shift (ACC)%04x = (ACC)%04x %s %x\n", res, ACC, arrow, distance))
		ACC <- res
	# load
	} else if (opcode == 0x3) {
		ACC <- MEMORY[address + 1]
		cat(sprintf("load  (ACC)%04x = [%03x]%04x\n", ACC, address, ACC))
	# store
	} else if (opcode == 0x4) {
		MEMORY[address + 1] <- ACC
		cat(sprintf("store [%03x]%04x = (ACC)%04x\n", address, ACC, ACC))
	# add
	} else if (opcode == 0x5) {
		val <- MEMORY[address + 1]
		res <- bitwAnd(ACC + val, 0xffff)
		cat(sprintf("add   (ACC)%04x = (ACC)%04x + [%03x]%04x\n", res, ACC, address, val))
		ACC <- res
	# sub
	} else if (opcode == 0x6) {
		val <- MEMORY[address + 1]
		res <- ACC - val
		if (res < 0) {
			res <- res + 0x10000
		}
		cat(sprintf("sub   (ACC)%04x = (ACC)%04x - [%03x]%04x\n", res, ACC, address, val))
		ACC <- res
	# and
	} else if (opcode == 0x7) {
		val <- MEMORY[address + 1]
		res <- bitwAnd(ACC, val)
		cat(sprintf("and   (ACC)%04x = (ACC)%04x & [%03x]%04x\n", res, ACC, address, val))
		ACC <- res
	# or
	} else if (opcode == 0x8) {
		val <- MEMORY[address + 1]
		res <- bitwOr(ACC, val)
		cat(sprintf("or    (ACC)%04x = (ACC)%04x | [%03x]%04x\n", res, ACC, address, val))
		ACC <- res
	# xor
	} else if (opcode == 0x9) {
		val <- MEMORY[address + 1]
		res <- bitwXor(ACC, val)
		cat(sprintf("xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x\n", res, ACC, address, val))
		ACC <- res
	# not
	} else if (opcode == 0xa) {
		res = bitwAnd(bitwNot(ACC), 0xffff)
		cat(sprintf("not   (ACC)%04x = ! (ACC)%04x\n", res, ACC))
		ACC <- res
	# nop
	} else if (opcode == 0xb) {
		cat("nop\n")
	# jmp
	} else if (opcode == 0xc) {
		cat(sprintf("jmp   [%03x]\n", address))
		PC <- address
	# jmpe
	} else if (opcode == 0xd) {
		if (ACC == 0) {
			cat(sprintf("jmpe  [%03x]\n", address))
			PC <- address
		} else {
			cat(sprintf("jmpe  (ACC)%04x\n", ACC))
		}
	# jmpl
	} else if (opcode == 0xe) {
		if (bitShiftR(ACC, 15) == 1) {
			cat(sprintf("jmpl  [%03x]\n", address))
			PC <- address
		} else {
			cat(sprintf("jmpl  (ACC)%04x\n", ACC))
		}
	# brl
	} else if (opcode == 0xf) {
		ACC <- PC
		PC <- address
		cat(sprintf("brl   [%03x]  (ACC)%04x\n", address, ACC))
	} else {
		write("Unknown opcode", stderr())
		quit(status = 1)
	}

	PC <- PC + 1
	if (PC > MEM_SIZE) {
		write(sprintf("Error: Memory overflow (PC = 0x%04x)", PC - 1), stderr())
		quit(status = 1)
	}
}
