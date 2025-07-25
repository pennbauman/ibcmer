#!/usr/bin/env python3
# IBCMer - Python
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
import sys
import string

MEM_SIZE = 4096
U16_MAX = 65536


def is_hex(s):
    return all(c in string.hexdigits for c in s)

class IBCM:
    acc = 0
    pc = 0
    memory = []

    def __init__(self, filepath):
        self.acc = 0
        self.pc = 0
        self.memory = [0]*MEM_SIZE

        try:
            with open(filepath) as source:
                lines = source.readlines()
        except:
            print("Error: Code file '" + filepath + "' not found", file=sys.stderr)
            sys.exit(1)

        addr = 0
        for line in lines:
            if addr >= MEM_SIZE:
                print("Error: Code file overflows memory (" + str(MEM_SIZE) + " lines max)", file=sys.stderr)
                sys.exit(1)
            if is_hex(line[0:4]):
                self.memory[addr] = int(line[0:4], 16)
                addr += 1
            else:
                i = 0
                while line[i] in string.hexdigits:
                    i += 1
                print("Error: '" + filepath + ":" + str(addr + 1) + ":" + str(i + 1) + "' Invalid opcode hexadecimal", file=sys.stderr)
                print(file=sys.stderr)
                print("    " + line, end="", file=sys.stderr)
                print("    " + " "*i + "^", file=sys.stderr)
                sys.exit(1)

    def print(self):
        print("ACC = {:04x}".format(self.acc))
        print("PC  =  {:03x}".format(self.pc))
        print("----------")
        mem_end = MEM_SIZE - 1
        while (self.memory[mem_end] == 0):
            mem_end -= 1
        for i in range(0, mem_end):
            print("[{:03x}] {:04x}".format(i, self.memory[i]))

    def step(self):
        if (self.pc >= MEM_SIZE):
            raise ValueError("Memory overflow (PC = 0x{:04x})".format(self.pc))
        opcode = self.memory[self.pc] >> 12
        address = self.memory[self.pc] & 0xfff
        print("[{:03x}]{:04x}  ".format(self.pc, self.memory[self.pc]), end="")
        # halt
        if (opcode == 0x0):
            print("halt")
            sys.exit(0)
        # i/o
        elif (opcode == 0x1):
            print("i/o   (ACC){:04x}".format(self.acc))
            action = address >> 8
            if (action == 0x0):
                tmp = input("Input hex:  ")
                if (not is_hex(tmp)) or (len(tmp) > 4):
                    print("Invalid hex input")
                    sys.exit(1)
                self.acc = int(tmp, 16)
            elif (action == 0x4):
                tmp = input("Input char: ")
                if (len(tmp) != 1):
                    print("Invalid char input")
                    sys.exit(1)
                self.acc = ord(tmp)
            elif (action == 0x8):
                print("Output hex:  {:04x}".format(self.acc))
            elif (action == 0xC):
                print("Output char: " + chr(self.acc))
            else:
                print("")
                raise ValueError("Invalid I/O sub-opcode '{:x}'".format(action))
        # shift
        elif (opcode == 0x2):
            prev_acc = self.acc
            action = address >> 8
            distance = address & 0x000f
            if (action == 0x0):
                self.acc = (self.acc << distance) % U16_MAX
                print("shift (ACC){:04x} = (ACC){:04x} << {:x}".format(
                        self.acc, prev_acc, distance))
            elif (action == 0x4):
                self.acc = self.acc >> distance
                print("shift (ACC){:04x} = (ACC){:04x} >> {:x}".format(
                        self.acc, prev_acc, distance))
            elif (action == 0x8):
                self.acc = ((self.acc << distance) % U16_MAX) | (self.acc >> (16 - distance))
                print("shift (ACC){:04x} = (ACC){:04x} <= {:x}".format(
                        self.acc, prev_acc, distance))
            elif (action == 0xC):
                self.acc = (self.acc >> distance) | ((self.acc << (16 - distance)) % U16_MAX)
                print("shift (ACC){:04x} = (ACC){:04x} => {:x}".format(
                        self.acc, prev_acc, distance))
            else:
                print("shift ")
                raise ValueError("Invalid shift sub-opcode '{:x}'".format(action))
        # load
        elif (opcode == 0x3):
            self.acc = self.memory[address]
            print("load  (ACC){:04x} = [{:03x}]{:04x}".format(self.acc, address, self.acc))
        # store
        elif (opcode == 0x4):
            self.memory[address] = self.acc
            print("store [{:03x}]{:04x} = (ACC){:04x}".format(address, self.acc, self.acc))
        # add
        elif (opcode == 0x5):
            prev_acc = self.acc
            self.acc = (self.acc + self.memory[address]) % U16_MAX
            print("add   (ACC){:04x} = (ACC){:04x} + [{:03x}]{:04x}".format(
                    self.acc, prev_acc, address, self.memory[address]))
        # sub
        elif (opcode == 0x6):
            prev_acc = self.acc
            self.acc = self.acc - self.memory[address]
            if (self.acc < 0):
                self.acc = U16_MAX + self.acc
            print("sub   (ACC){:04x} = (ACC){:04x} - [{:03x}]{:04x}".format(
                    self.acc, prev_acc, address, self.memory[address]))
        # and
        elif (opcode == 0x7):
            prev_acc = self.acc
            self.acc = self.acc & self.memory[address]
            print("and   (ACC){:04x} = (ACC){:04x} & [{:03x}]{:04x}".format(
                    self.acc, prev_acc, address, self.memory[address]))
        # or
        elif (opcode == 0x8):
            prev_acc = self.acc
            self.acc = self.acc | self.memory[address]
            print("or    (ACC){:04x} = (ACC){:04x} | [{:03x}]{:04x}".format(
                    self.acc, prev_acc, address, self.memory[address]))
        # xor
        elif (opcode == 0x9):
            prev_acc = self.acc
            self.acc = self.acc ^ self.memory[address]
            print("xor   (ACC){:04x} = (ACC){:04x} ^ [{:03x}]{:04x}".format(
                    self.acc, prev_acc, address, self.memory[address]))
        # not
        elif (opcode == 0xA):
            prev_acc = self.acc
            self.acc = self.acc ^ 0xffff
            print("not   (ACC){:04x} = ! (ACC){:04x}".format(self.acc, prev_acc))
        # nop
        elif (opcode == 0xB):
            print("nop")
        # jmp
        elif (opcode == 0xC):
            print("jmp   [{:03x}]".format(address))
            self.pc = address - 1
        # jmpe
        elif (opcode == 0xD):
            if (self.acc == 0):
                print("jmpe  [{:03x}]".format(address))
                self.pc = address - 1
            else:
                print("jmpe  (ACC){:04x}".format(self.acc))
        # jmpl
        elif (opcode == 0xE):
            if (self.acc & 0x8000):
                print("jmpl  [{:03x}]".format(address))
                self.pc = address - 1
            else:
                print("jmpl  (ACC){:04x}".format(self.acc))
        # brl
        elif (opcode == 0xF):
            print("brl   [{:03x}]  (ACC){:04x}".format(address, self.pc + 1))
            self.acc = self.pc + 1
            self.pc = address - 1
        else:
            print("\nUnknown operation code")
            sys.exit(1)
        self.pc += 1


# Load and Run Program
if (len(sys.argv) < 2):
    print("Error: A code file must be provided", file=sys.stderr)
    sys.exit(1)

executor = IBCM(sys.argv[1])
while (True):
    try:
        executor.step()
    except ValueError as e:
        print("Error:", e, file=sys.stderr)
        sys.exit(1)
