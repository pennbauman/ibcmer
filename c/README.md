# IBCMer in C
Command line executor and debugger for IBCM written in C


## Building and Testing
To build ibcmer run:

    make

To run tests use:

    make test


## Installation
To install in /usr/local (requires root access) run:

	make install

To install to another location add `prefix=[location]`, for example:

	make install prefix=~/.local


## Usage

	ibcmer [code-file.ibcm] [options]

### Options
- `--version` Prints version information, then exits.

- `-h`, `--help` Prints help information, then exits.

- `-i`, `--ibcm-info` Prints information about IBCM, then exits.

- `-c`, `--check` Checks the code file for proper line numbers.

- `-q`, `--quiet` Prevents printing of detailed program output during execution.

- `--silent` Prevents printing of anything except program output, even input prompts and newlines after output. Useful for running program with automated input.

- `-s`, `--step` Starts the program in debugging mode.

- `-b [point(s)]`, `--break [point(s)]` Sets breakpoint(s) to enter debugging mode. A single points can be provided as a single hexadecimal number. Multiple points can be added by providing a comma separated list of hexadecimal numbers, without spaces.

### Debug Commands
Used when in debug mode. If no command is given step in run.

- `exit` Exits the program.

- `run` Exits debugging mode and runs the program normally.

- `step [number]` Executes the provided number of steps of the program and then return to debugging. The number must be in decimal format and if no number is provided 1 is used.

-  `view [all|address|address-address]` Print the contents of memory. If a single hexadecimal address is provided, that memory slot is printed. If two address are provided separated by a `-` the range between them is printed. If `all` is provided then the range of memory containing values is printed.

- `set [address] [value]` Sets the value of a memory slot, both the address and the value are provided as hexadecimal numbers.
