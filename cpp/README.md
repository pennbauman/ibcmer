# IBCMer in C++
Command line executor and debugger for IBCM written in C++


## Installation
To install in /usr/local (requires root access) run:

	make install

To install to another location add `prefix=[location]`, for example:

	make install prefix=~/.local


## Usage
This command line tool is designed to run IBCM code while providing feedback and debugging tools. Once installed .ibcm files can be run using:

	ibcmer [file] [options]

#### Stand Alone Options
These options will preform there function and then exit the program preventing it from running.

- `-h`, `--help` Prints out a help menu listing command line options and debugging commands.

- `--version` Prints out version number.

#### Regular Options
These options will effect the way IBCMer read and executes files. They must be listed after the code file.

- `--strict` Prevents the program from ignoring empty lines.

- `-c`, `--check` Checks for proper line numbering (3 digit in hexadecimal immediately after the op-code). Implies `--strict`.

- `-b <number> [numbers...]` Sets a break-point at the listed line number. Multiple numbers can be specified for multiple break-points. All numbers should be given in hexadecimal.

- `--step` Steps through each line individually. Effectively places a break-point at line 0.

- `-q`, `--quiet` Removes detailed output outside of debugging.

#### Break-Point Commands
These command can be used in debugging mode.

- `run` Runs the program as normal (until the next break-point).

- `step [number]` Runs a number of lines and then returns to debugging mode. Will default to one line if a number is not specified.

- `view [address|all]` By default, this prints out the contents of all occupied memory addresses. If an address is provide, this prints out the contents of the given memory address (specified in hex). If `all` is specified, this prints out the contents of all memory addresses.

- `set <address> <value>` Sets the value of the given address (both specified in hex).

- `exit` Ends program immediately.
