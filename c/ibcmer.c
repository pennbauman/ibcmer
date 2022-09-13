// IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Include other components
#include "src/text.h" // Defines text variables
#include "src/executor.h" // IBCM executor
#include "src/debugger.h" // debugging tools

// Version number
#define VERSION "0.8.0"


// Global variables
ibcmemory DATA; // ICBM executor data
breakpoints BREAKS; // debug breakpoints list
// Options
unsigned char OPT_CHECK = 0; // Option to check line numbers
unsigned char OPT_VOLUME = 2; // Option to change amount of printing
	// 0 = completely silent
	// 1 = print only input prompts and output with newlines
	// 2 = print everthing (stuff for 1 and command details)
unsigned char DEBUG_STATE = 0; // If debugging is active



int main(int argc, char **argv) {
	// Setup global structs
	BREAKS = init_breakpoints();

	// Read through command line arguements
	char* src_path = "";
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			// --version
			if (0 == strcmp("--version", argv[i])) {
				printf("IBCMer v%s\n", VERSION);
				return 0;
			// --help, -h
			} else if ((0 == strcmp("--help", argv[i])) ||
					(0 == strcmp("-h", argv[i]))) {
				printf("%s\n", HELP);
				return 0;
			// --ibcm-info, -i
			} else if ((0 == strcmp("--ibcm-info", argv[i])) ||
					(0 == strcmp("-i", argv[i]))) {
				printf("%s\n", HELP_IBCM);
				return 0;
			// --check, -c
			} else if ((0 == strcmp("--check", argv[i])) ||
					(0 == strcmp("-c", argv[i]))) {
				OPT_CHECK = 1;
			// --quiet, -q
			} else if ((0 == strcmp("--quiet", argv[i])) ||
					(0 == strcmp("-q", argv[i]))) {
				OPT_VOLUME = 1;
			// -- silent
			} else if (0 == strcmp("--silent", argv[i])) {
				OPT_VOLUME = 0;
			// --step, -s
			} else if ((0 == strcmp("--step", argv[i])) ||
					(0 == strcmp("-s", argv[i]))) {
				add_breakpoint(&BREAKS, 0);
			// --break, -b [point(s)]
			} else if ((0 == strcmp("--break", argv[i])) ||
					(0 == strcmp("-b", argv[i]))) {
				i++;
				signed char is_num = 0;
				// Check next arguement is all hex and commas
				for (int j = 0; j < strlen(argv[i]); j++) {
					if (isxdigit(argv[i][j])) {
						continue;
					} else if (argv[i][j] == ',') {
						is_num = 1;
					} else {
						is_num = 2;
						break;
					}
				}
				// Error for bad formatting
				if (is_num == 2) {
					printf("%s Invalid breakpoint '%s'\n", E_ERROR, argv[i]);
					return 1;
				} else if (is_num == 1) {
					char num[4];
					int k = 0;
					// Read numbers from list
					for (int j = 0; j < strlen(argv[i])+1; j++) {
						if (! isxdigit(argv[i][j])) {
							// Check for adjacent commas
							if (k == 0) {
								printf("%s Invalid breakpoints '%s'\n",
										E_ERROR, argv[i]);
								return 1;
							}
							num[k] = '\0';
							add_breakpoint(&BREAKS, strtol(num, NULL, 16));
							k = 0;
						} else {
							num[k] = argv[i][j];
							k++;
						}
					}
				} else {
					add_breakpoint(&BREAKS, strtol(argv[i], NULL, 16));
				}
			} else {
				printf("%s Unknown options '%s'\n", E_ERROR, argv[i]);
				return 1;
			}
		// Get code file path
		} else {
			if (src_path[0] == '\0') {
				src_path = argv[i];
			} else {
				printf("%s Multiple code files provided, only one allowed\n",
						E_ERROR);
				return 1;
			}
		}
	}
	
	if (read_file(&DATA, src_path, OPT_CHECK))
		return 1;

	// Run program
	while (1) {
		// Check breakpoints
		if (is_breakpoint(&BREAKS, DATA.pc))
			DEBUG_STATE = 1;
		// Get debug command if necessary
		if (DEBUG_STATE) {
			printf("%sdebug[%s%03x%s]:%s ", C_BLUE, C_NONE, DATA.pc, C_BLUE, C_NONE);
			char input[64];
			fgets(input, 64, stdin);
			if (feof(stdin)) {
				printf("exit\n");
				return 0;
			}
			DEBUG_STATE = debug(&DATA, &BREAKS, input, OPT_VOLUME);
		// Otherwise continue program
		} else {
			step(&DATA, OPT_VOLUME);
		}
	}
	return 0;
}
