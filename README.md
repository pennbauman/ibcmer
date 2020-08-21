# IBCMer
A command line IBCM executor and debugger written in C


## Installation
Run the installation script, which requires root privileges:

	./install.sh


## Usage

	ibcmer [code-file.ibcm] [options]

### Options
- `--version` Prints version information, then exits.

- `-h`, `--help` Prints help information, then exits.

- `--help-ibcm` Prints information about IBCM.

- `-c`, `--check` Checks the code file for proper line numbers.

- `-q`, `--quiet` Prevents printing of detailed program output during execution.

- `-s`, `--step` Starts the program in debugging mode.

- `-b [points]`, `--break [points]` Sets breakpoint(s) to enter debugging mode. A single points is provided as a single hexadecimal number. Multiple points can be added by providing a comma separated list of hexadecimal numbers (no spaces).

### Debug Commands
- `exit` Exits the program.

- `run` Exits debugging mode and runs the program normally.

- `step` Executes one step of the program.

-  `view [address]` Print the contents of memory. If a single hexadecimal address is provided, that memory slot is printed. If two address are provided separated by a `-` the range between them is printed. If `all` is provided then the range of memory containing values is printed.

- `set [address] [value]` Sets the value of a memory slot, both the address and the value are provided as hexadecimal numbers.


## IBCM
The Itty Bitty Computing Machine (IBCM) is a mock machine language design at The University of Virginia for use in introductory computer science classes. An online simulator and further details can be found at [pegasus.cs.virginia.edu/ibcm](http://pegasus.cs.virginia.edu/ibcm/index.html). All IBCM documentation and source code is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/).

IBCM runs with 4096 16 bit memory slots, a 16 bit accumulator, and a 12 bit program counter. All memory, the accumulator, and the counter are initialized to zero and code is then loaded into memory. When executed it runs the command in the memory slot indicated by the program counter (see below) and increments the counter, repeating until it reaches a halt command.

IBCM code files (typically .ibcm files) are formatted so that the first 4 characters of each line repentant the 16 bits of each memory slots in hexadecimal (all following characters are ignored). These values are loaded into memory when the executor starts and are used both as the instructions for the program and the data it runs on. All characters after the first 4 on each line are not used for execution but line numbers (starting at 000) are commonly placed directly after the commands.

### IBCM Operation Codes
The type of command is indicated by the first 4 bits (the first hexadecimal digit) and is listed in the title of each subsection. The following bit are used in varying ways, indicated by bulleted sections (further indentation indicates later bits).

#### `0` Halt
Exit Program and cease execution.
- `000` Unused

#### `1` Input & Output (i/o)
Input to or Output from the accumulator.
- `0` Input a hexadecimal number to the accumulator
- `4` Input a ASCII character to the accumulator
- `8` Print the hexadecimal value of the accumulator
- `c` Print the ASCII character from the accumulator
	- `00` Unused

#### `2` Shift
Shift the bits of the accumulator.
- `0` Shift the bits left and fill empty space with zeros
- `4` Shift the bits right and fill empty space with zeros
- `8` Rotate the bits left, wrapping moved values around
- `C` Rotate the bits right, wrapping moved values around
	- `0` Unused
		- `#` The distance to move the bits

#### `3` Load
Set the value of the accumulator to values stored at the address.
- `###` Address to load form

#### `4` Store
Set the values at the address to the value of the accumulator.
- `###` Address to store to

#### `5` Add
Add the value at the address from the value of the accumulator.
- `###` Address to add from

#### `6` Subtract (sub)
Subtract the value at the address from the value of the accumulator.
- `###` Address to subtract from

#### `7` AND
Apply a bitwise AND to the value of the accumulator and the value at the address.
- `###` Address to AND with

#### `8` OR
Apply a bitwise OR to the value of the accumulator and the value at the address.
- `###` Address to OR with

#### `9` XOR
Apply a bitwise XOR to the value of the accumulator and the value at the address.
- `###` Address to XOR with

#### `A` NOT
Apply bitwise NOT to the value of the accumulator.
- `000` Unused

#### `B` No Operation (nop)
Do nothing.
- `000` Unused

#### `C` Jump (jmp)
Jump the program counter to the address.
- `###` Address to jump to

#### `D` Jump if Equal (jmpe)
Jump the program counter to the address if the value of the accumulator is zero.
- `###` Address to jump to

#### `E` Jump if Less Than (jmpl)
Jump the program counter to the address if the value of the accumulator is negative.
- `###` Address to jump to

#### `F` Branch & Link (brl)
Set the value of the accumulator to the program counter plus 1 and jump to address.
- `###` Address to jump to


## Sources & License
This project and all code it contains is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/). Files within the tests/ directory are written by [Aaron Bloomfield](https://github.com/aaronbloomfield) and are also available at [github.com/uva-cs/pdr](https://github.com/uva-cs/pdr/tree/master/ibcm).
