# IBCMer
A command line IBCM simulator written in C++


## IBCM
The Itty Bitty Computing Machine (IBCM) is a mock machine language design at The University of Virginia for use in introductory computer science classes. An online simulator can be found at [pegasus.cs.virginia.edu/ibcm](http://pegasus.cs.virginia.edu/ibcm/index.html). All source code is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/) and is available at [github.com/uva-cs/pdr](https://github.com/uva-cs/pdr/tree/master/ibcm).

IBCM runs with 4096 16 bit memory slots (numbered 000 to fff), a 16 bit accumulator, and a program counter. All memory, the accumulator, and the counter are initialized to zero. When started it runs the command in the memory slot indicated by the counter and increments the counter, repeating until it reaches a halt command.

.ibcm files are formated so that the first 4 characters of each line repersent the 16 bits of each memory slots in hexadecimal (all following characters are typically ignored). These values are loaded into memory when the program starts and are used both as the instructions for the program and the data it runs on. The operation codes for IBCM are listed in [ibcm-op-codes.txt](ibcm-op-codes.txt).


## Installation
On linux run the following in the ibcmer directory, it requires root access:

	./install.sh


## Usage
This command line tool is designed to run IBCM code while providing feedback and debugging tools. Once installed .ibcm files can be run using:

	ibcmer [file] [options]

#### Stand Alone Options
These options will preform there function and then exit the program preventing it from running.

`-h, --help`
Prints out a help menu listing command line options and debugging commands.

`--version`
Prints out version information and memory slots.

#### Regular Options
These options will effect the way IBCMer read and executes files. They must be listed after the code file.

`--strict`
Prevents the program from ignoring empty lines.

`-c, --check`
Checks for proper line numbering (3 digit in hexadecimal immediately after the op-code). Implies `--strict`.

`-b <number> [numbers...]`
Sets a break-point at the listed number. Multiple numbers can be specified for multiple break-points.

`--step`
Steps through each line individually. Effectively places a break-point at line 0.

`-q, --quiet`
Removes detailed output outside of debugging.

#### Break-Point Commands
These command can be used in debugging mode.

`run`
Runs to program as normal (until the next break-point).

`step [number]`
Runs a number of lines and then returns to debugging mode. Will default to one line if not specified.

`view [address]`
Prints out the contents of the memory address (specified in hex). Prints out all of memory if no address is specified.

`set <address> <value>`
Sets the value of the given address (both specified in hex).

`exit`
Ends program immediately.

## Sources & License
This project and all code it contains is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/). Files within the tests/ directory are written by [Aaron Bloomfield](https://github.com/aaronbloomfield) and are also available at [github.com/uva-cs/pdr](https://github.com/uva-cs/pdr/tree/master/ibcm).
