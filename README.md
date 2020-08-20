# IBCMer
A command line IBCM executor written in C

## Installation
Run the installation script, which requires root privileges:

	./install.sh

## Usage

	ibcmer [code-file.ibcm] [options]

### Options
- `--version` Prints version information, then exits.

- `-h`, `--help` Prints help information, then exits.

- `-c`, `--check` Checks the code file for proper line numbers.

- `-q`, `--quiet` Prevents printing of detailed program output during execution.

## Sources & License
This project and all code it contains is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/). Files within the tests/ directory are written by [Aaron Bloomfield](https://github.com/aaronbloomfield) and are also available at [github.com/uva-cs/pdr](https://github.com/uva-cs/pdr/tree/master/ibcm).
