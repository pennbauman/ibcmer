# IBCMer

Command line IBCM simulator written in C++

## IBCM

The Itty Bitty Computing Machine (IBCM) is a mock machine language design at The University of Virginia for use in introductory computer science classes. An online simulator can be found at [pegasus.cs.virginia.edu/ibcm](http://pegasus.cs.virginia.edu/ibcm/index.html). All source code is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/) and is available at [github.com/uva-cs/pdr](https://github.com/uva-cs/pdr).

## Overview

This command line tool is designed to run IBCM code while providing feedback and debugging tools. Once installed .ibcm files can be run using:

	ibcmer code.ibcm

This project and all code it contains is licensed under the [Creative Commons Attribution-ShareAlike 4.0 International](http://creativecommons.org/licenses/by-sa/4.0/). Files within the tests/ directory are written by [Aaron Bloomfield](https://github.com/aaronbloomfield) and are also available at [github.com/uva-cs/pdr](https://github.com/uva-cs/pdr/tree/master/ibcm).

## Install
Run the following in the ibcmer directory, it requires root:

	./install.sh

## Usage

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

`-b <number>`
Sets a break-point at the listed number.
