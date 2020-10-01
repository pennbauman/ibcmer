// IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
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



// Check line numbers are properly formatted
signed char check_line_num(char* line, unsigned short line_num) {
	int i = 4;
	// Check there are spaces after the command
	if ((line[i] != ' ') && (line[i] != '\t'))
		return i;
	// Skip over whitespace
	while ((line[i] == ' ') || (line[i] == '\t'))
		i++;
	//Pasre number
	int j = 0;
	char num[4];
	while ((line[i+j] != ' ') && (line[i+j] != '\t')) {
		if (! isxdigit(line[i + j]))
			return i + j;
		if (j > 2)
			return i + j;
		num[j] = line[i + j];
		j++;
	}
	num[j] = '\0';
	// Compare to expected number
	if (line_num != strtol(num, NULL, 16))
		return 1;
	return 0;
}


int main(int argc, char **argv) {
	// Setup global structs
	BREAKS = init_breakpoints();
	DATA = init_ibcmemory();
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

	// Open code file
	if (strlen(src_path) == 0) {
		printf("%s A code file must be provided\n", E_ERROR);
		return 1;
	}
	// Check code file exists
	FILE *src = fopen(src_path, "r");
	if (src == NULL) {
		printf("%s Code file '%s' not found\n", E_ERROR, src_path);
		return 1;
	}
	// Read from code file
	char ch, line[256];
	unsigned int check, num = 0;
	int i = 0;
	while ((ch = fgetc(src)) != EOF) {
		if (ch == '\n') {
			line[i] = '\0';
			// Check validity of code line
			int j = 0;
			for (; j < 4; j++) {
				if (! isxdigit(line[j])) {
					printf("%s '%s:%d:%d' Invalid operation code\n",
							E_ERROR, src_path, num + 1, j + 1);
					printf("\n    %s\n    ", line);
					for (int k = 0; k < j; k++)
						printf(" ");
					printf("%s^%s\n", C_YELLOW, C_NONE);
					return 1;
				}
			}
			// Check for invalid line number
			if (OPT_CHECK) {
				unsigned char test = check_line_num(line, num);
				if (test == 1) {
					printf("%s '%s:%d' Incorrect line number\n",
							E_ERROR, src_path, num + 1);
					printf("\n    %s\n        ", line);
					int k = 4;
					while ((line[k] == ' ') || (line[k] == '\t')) {
						printf(" ");
						k++;
					}
					printf("%s", C_YELLOW);
					//k++;
					while ((line[k] != ' ') && (line[k] != '\t')) {
						printf("^");
						k++;
					}
					printf("%s\n", C_NONE);
					return 1;

				}
				if (test > 2) {
					printf("%s '%s:%d:%d' Invalid line number\n",
							E_ERROR, src_path, num + 1, test + 1);
					printf("\n    %s\n    ", line);
					for (int k = 0; k < test; k++)
						printf(" ");
					printf("%s^%s\n", C_YELLOW, C_NONE);
					return 1;
				}
			}
			// Parse command and save it to memory
			char hex[5];
			for (int i = 0; i < 4; i++) {
				hex[i] = line[i];
			}
			hex[4] = '\0';
			DATA.mem[num] = strtol(hex, NULL, 16);
			num++;
			i = 0;
		} else {
			line[i++] = ch;
		}
	}

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
