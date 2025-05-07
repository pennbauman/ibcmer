# IBCMer
Executors and debuggers for the IBCM mock ISA implemented in various languages


## Building & Testing
Dependencies can be install for Fedora (or for Debian although missing Zig) by running:

    ./dependencies.sh

To build all implementations and run all tests use:

    make
    make test

For running specific tests you can use `./tests/run.sh` directly, for more information run:

    ./tests/run.sh --help


## Purpose
I wrote the first implementation of IBCMer (in C++) while taking [CS 2150](https://uva-cs.github.io/pdr) at [UVA](https://www.virginia.edu). The IBCM executor provided by the class was cumbersome and required internet access, so I created this tool to allow for quick local testing of IBCM programs. Since then I have reimplemented IBCMer in several languages for fun and to help practice new languages I am learning.


## IBCM
The Itty Bitty Computing Machine (IBCM) is a mock machine language design at The University of Virginia for use in introductory computer science classes. An online simulator and further details can be found at [pegasus.cs.virginia.edu/ibcm](https://pegasus.cs.virginia.edu/ibcm/index.html). All IBCM documentation and source code is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International License](https://creativecommons.org/licenses/by-sa/4.0/).

IBCM runs with 4096 16 bit memory slots, a 16 bit accumulator, and a 12 bit program counter. All memory, the accumulator, and the counter are initialized to zero and code is then loaded into memory (see below). When executed it runs the command in the memory slot indicated by the program counter and increments the counter, repeating until it reaches a halt command.

IBCM code files (typically .ibcm files, see [examples](examples)) are formatted so that the first 4 characters of each line represent the 16 bits of each memory slots in hexadecimal. These values are loaded into memory when the executor starts and are used both as the instructions for the program and the data it runs on. All characters after the first 4 on each line are not used for execution but line numbers (starting at 000) are commonly placed directly after the commands.

### IBCM Operation Codes
The type of command is indicated by the first 4 bits (the first hexadecimal digit) and is listed in the title of each subsection. The later bit are used in varying ways depending on the command, as described in each commands table.

#### `0` Halt
Exit Program and halt execution.

| `0` | `000` |
|:---:|:------|
| Command | Unused |

#### `1` Input & Output (i/o)
Input to or Output from the accumulator.

| `1` | `-` | `00` |
|:---:|:----|:----|
| Command | `0` Input a hexadecimal number to the accumulator<br/>`4` Input a ASCII character to the accumulator<br/>`8` Print the hexadecimal value of the accumulator<br/>`c` Print the ASCII character from the accumulator | Unused |

#### `2` Shift
Shift the bits of the accumulator.

| `2` | `-` | `0` | `-` |
|:---:|:----|:----|:----|
| Command | `0` Shift the bits left and fill empty space with zeros<br/>`4` Shift the bits right and fill empty space with zeros<br/>`8` Rotate the bits left, wrapping moved values around<br/>`C` Rotate the bits right, wrapping moved values around | Unused | Distance to move the bits |

#### `3` Load
Set the value of the accumulator to values stored at the address.

| `3` | `---` |
|:---:|:------|
| Command | Address to load from |

#### `4` Store
Set the values at the address to the value of the accumulator.

| `4` | `---` |
|:---:|:------|
| Command | Address to store to |

#### `5` Add
Add the value at the address from the value of the accumulator.

| `5` | `---` |
|:---:|:------|
| Command | Address of value to add from |

#### `6` Subtract (sub)
Subtract the value at the address from the value of the accumulator.

| `6` | `---` |
|:---:|:------|
| Command | Address of value to subtract from |

#### `7` AND
Apply a bitwise AND to the value of the accumulator and the value at the address.

| `7` | `---` |
|:---:|:------|
| Command | Address of value to AND with |

#### `8` OR
Apply a bitwise OR to the value of the accumulator and the value at the address.

| `8` | `---` |
|:---:|:------|
| Command | Address of value to OR with |

#### `9` XOR
Apply a bitwise XOR to the value of the accumulator and the value at the address.

| `9` | `---` |
|:---:|:------|
| Command | Address of value to XOR with |

#### `A` NOT
Apply bitwise NOT to the value of the accumulator.

| `A` | `000` |
|:---:|:------|
| Command | Unused |

#### `B` No Operation (nop)
Do nothing.

| `B` | `000` |
|:---:|:------|
| Command | Unused |

#### `C` Jump (jmp)
Jump the program counter to the address.

| `C` | `---` |
|:---:|:------|
| Command | Address to jump to |

#### `D` Jump if Equal (jmpe)
Jump the program counter to the address if the value of the accumulator is zero.

| `D` | `---` |
|:---:|:------|
| Command | Address to jump to |

#### `E` Jump if Less Than (jmpl)
Jump the program counter to the address if the value of the accumulator is negative.

| `E` | `---` |
|:---:|:------|
| Command | Address to jump to |

#### `F` Branch & Link (brl)
Set the value of the accumulator to the program counter plus 1 and jump to the address.

| `F` | `---` |
|:---:|:------|
| Command | Address to jump to |
