// IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Defines
#include "text.h" // Defines text variables
#define MEM_SIZE 4096 // Size of IBCM memory
#define VERSION "0.7.4" // Version number


// Global variables
unsigned short ACC = 0; // Accumulator
unsigned short PC = 0; // Program counter
unsigned short MEM[MEM_SIZE]; // Program memory
// Options
unsigned char OPT_CHECK = 0; // Option to check line numbers
unsigned char OPT_VOLUME = 2; // Option to change amount of printing
	// 0 = completely silent
	// 1 = print only input prompts and output with newlines
	// 2 = print everthing (stuff for 1 and command details)
// Debug variables
unsigned char DEBUG_STEP = 0; // If debugging is active
unsigned short *DEBUG_BREAKPOINTS; // Pointer to breakpoints array
unsigned short DEBUG_BREAKS_COUNT = 0; // Length of breakpoints list
unsigned short DEBUG_BREAKS_MAX = 0; // Length if breakpoints array



// Check if breakpoint exists
signed char is_breakpoint(unsigned short num) {
	for (int i = 0; i < DEBUG_BREAKS_COUNT; i++) {
		if (DEBUG_BREAKPOINTS[i] == num)
			return 4;
	}
	return 0;
}
// Add new breakpoint
void add_breakpoint(unsigned short num) {
	// Skip duplicates
	if (is_breakpoint(num))
		return;
	// Create breakpoint array if necessary
	if (DEBUG_BREAKS_MAX == 0) {
		DEBUG_BREAKPOINTS = malloc(sizeof(short)*16);
		DEBUG_BREAKS_MAX = 16;
	}
	// Grows breakpoint vector possibly copying array
	DEBUG_BREAKS_COUNT++;
	if (DEBUG_BREAKS_COUNT == DEBUG_BREAKS_MAX) {
		unsigned short *temp = malloc(sizeof(short)*DEBUG_BREAKS_MAX*2);
		for (int i = 0; i < DEBUG_BREAKS_MAX; i++) {
			temp[i] = DEBUG_BREAKPOINTS[i];
		}
		DEBUG_BREAKS_MAX *= 2;
		free(DEBUG_BREAKPOINTS);
		DEBUG_BREAKPOINTS = temp;
	}
	// Writes new breakpoints
	DEBUG_BREAKPOINTS[DEBUG_BREAKS_COUNT-1] = num;
}

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

// Execute one command and progess the program
void step(int volume) {
	unsigned short temp = 0;
	// Check for memory overflow
	if (PC == MEM_SIZE) {
		printf("%s Program overran memory\n", E_ERROR);
		exit(1);
	}
	if (volume > 1)
		printf("[%03x]%04x  ", PC, MEM[PC]);
	// Setup address value
	unsigned short address = MEM[PC] << 4;
	address = address >> 4;
	// Check command
	switch (MEM[PC] >> 12) {
		// Halt
		case 0:
			if (volume > 1)
				printf("halt\n");
			exit(0);
		// I/O
		case 1:
			if (volume > 1)
				printf("i/o\n");
			unsigned char type = MEM[PC] >> 8;
			type = type << 4;
			type = type >> 4;
			char input[64];
			// Check I/O type
			switch (type) {
				// Hex input
				case 0:
					while (1) {
						if (volume > 0)
							printf("Input hex: ");
						fgets(input, 64, stdin);
						// Check length is 4
						if (strlen(input) > 5) {
							printf("  %s Too long\n", E_INVALID_IN);
						} else if (strlen(input) < 2) {
							printf("  %s\n", E_MISSING_IN);
						} else {
							// Check all characters are valid hex
							char valid = 1;
							for (int i = 0; i < strlen(input) - 1; i++) {
								if (! isxdigit(input[i]))
									valid = 0;
							}
							// Read hex value
							if (valid) {
								ACC = strtol(input, NULL, 16);
								if (volume > 1)
									printf("  [ACC]%04x\n", ACC);
								break;
							} else {
								printf("  %s Not hex\n", E_INVALID_IN);
							}
						}
					}
					break;
				case 4:
				// Char input
					while (1) {
						if (volume > 0)
							printf("Input char: ");
						fgets(input, 64, stdin);
						// Check length is 1
						if (strlen(input) > 2) {
							printf("  %s Multiple characters\n", E_INVALID_IN);
						} else if (strlen(input) < 2) {
							printf("  %s\n", E_MISSING_IN);
						} else {
							// Read char value
							ACC = input[0];
							if (volume > 1)
								printf("  [ACC]%04x\n", ACC);
							break;
						}
					}
					break;
				// Hex ouput
				case 8:
					if (volume > 1)
						printf("Output: ");
					printf("%04x", ACC);
					if (volume > 0)
						printf("\n");
					break;
				// Char ouput
				case 12:
					if (volume > 1)
						printf("Output: ");
					printf("%c", ACC);
					if (volume > 0)
						printf("\n");
					break;
				// Check invalid subcommands
				default:
					printf("\n%s Invalid I/O operation code\n", E_ERROR);
					exit(1);
			}
			break;
		// Shift
		case 2:
			if (volume > 1)
				printf("shift ");
			// Get direction value
			unsigned char direction = MEM[PC] >> 8;
			direction = direction << 4;
			direction = direction >> 4;
			// Get distance value
			unsigned char distance = MEM[PC] << 4;
			distance = distance >> 4;
			switch (direction) {
				// Shift left, insert 0s
				case 0:
					temp = ACC << distance;
					if (volume > 1)
						printf("[ACC]%04x = [ACC]%04x << %x\n",
								temp, ACC, distance);
					break;
				// Shift right, insert 0s
				case 4:
					temp = ACC >> distance;
					if (volume > 1)
						printf("[ACC]%04x = [ACC]%04x >> %x\n",
								temp, ACC, distance);
					break;
				// Rotate left, wrap
				case 8:
					temp = (ACC << distance) + (ACC >> (16 - distance));
					if (volume > 1)
						printf("[ACC]%04x = [ACC]%04x <= %x\n",
								temp, ACC, distance);
					break;
				// Rotate right, wrap
				case 12:
					temp = (ACC >> distance) + (ACC << (16 - distance));
					if (volume > 1)
						printf("[ACC]%04x = [ACC]%04x => %x\n",
								temp, ACC, distance);
					break;
				// Check invalid subcommands
				default:
					printf("\n%s Invalid shift operation code\n", E_ERROR);
					exit(1);
			}
			ACC = temp;
			break;
		// Load value
		case 3:
			if (volume > 1)
				printf("load  [ACC]%04x\n", ACC);
			ACC = MEM[address];
			break;
		// Store value
		case 4:
			if (volume > 1)
				printf("store [%03x]%04x\n", address, ACC);
			MEM[address] = ACC;
			break;
		// Add
		case 5:
			temp = ACC + MEM[address];
			if (volume > 1)
				printf("add   [ACC]%04x = [ACC]%04x + [%03x]%04x\n",
						temp, ACC, address, MEM[address]);
			ACC = temp;
			break;
		// Subtract
		case 6:
			temp = ACC - MEM[address];
			if (volume > 1)
				printf("sub   [ACC]%04x = [ACC]%04x - [%03x]%04x\n",
						temp, ACC, address, MEM[address]);
			ACC = temp;
			break;
		// AND
		case 7:
			temp = ACC & MEM[address];
			if (volume > 1)
				printf("and   [ACC]%04x = [ACC]%04x & [%03x]%04x\n",
						temp, ACC, address, MEM[address]);
			ACC = temp;
			break;
		// OR
		case 8:
			temp = ACC | MEM[address];
			if (volume > 1)
				printf("or    [ACC]%04x = [ACC]%04x | [%03x]%04x\n",
						temp, ACC, address, MEM[address]);
			ACC = temp;
			break;
		// XOR
		case 9:
			temp = ACC ^ MEM[address];
			if (volume > 1)
				printf("xor   [ACC]%04x = [ACC]%04x ^ [%03x]%04x\n",
						temp, ACC, address, MEM[address]);
			ACC = temp;
			break;
		// NOT
		case 10:
			temp = ~ACC;
			if (volume > 1) {
				printf("not   [ACC]%04x = ~[ACC]%04x\n", temp, ACC);
			}
			ACC = temp;
			break;
		// Nothing
		case 11:
			if (volume > 1)
				printf("nop\n");
			break;
		// Jump
		case 12:
			if (volume > 1)
				printf("jmp   [%03x]\n", address);
			PC = address - 1;
			break;
		// Jump, ACC = 0
		case 13:
			if (volume > 1)
				printf("jmpe  ");
			if (ACC == 0) {
				if (volume > 1)
					printf("[%03x]\n", address);
				PC = address - 1;
			} else {
				if (volume > 1)
					printf("[ACC]%04x\n", ACC);
			}
			break;
		// Jump, ACC < 0
		case 14:
			if (volume > 1)
				printf("jmpl  ");
			if (ACC >> 15 == 1) {
				if (volume > 1)
					printf("[%03x]\n", address);
				PC = address - 1;
			} else {
				if (volume > 1)
					printf("[ACC]%04x\n", ACC);
			}
			break;
		// Branch & Link
		case 15:
			if (volume > 1)
				printf("brl   [%03x]  [ACC]%04x\n", address, PC + 1);
			ACC = PC + 1;
			PC = address - 1;
			break;
	}
	PC++;
}

// Run debug commands
void debug(char *cmd) {
	unsigned short i = 0;
	while (cmd[i] == ' ')
		cmd = &cmd[1];
	// Step command
	if (0 == strcmp("\n", cmd)) {
		step(2);
	} else if ((cmd[0] == 's') && (cmd[1] == 't') &&
			(cmd[2] == 'e') && (cmd[3] == 'p')) {
		i = 4;
		while (cmd[i] == ' ')
			i++;
		if ((cmd[i] != '\n') && (i == 4)) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return;
		}
		step(2);

	// Run command
	} else if ((cmd[0] == 'r') && (cmd[1] == 'u') &&
			(cmd[2] == 'n')) {
		i = 3;
		while (cmd[i] == ' ')
			i++;
		if ((cmd[i] != '\n') && (i == 3)) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return;
		}
		DEBUG_STEP = 0;
		step(OPT_VOLUME);

	// Exit command
	} else if ((cmd[0] == 'e') && (cmd[1] == 'x') &&
			(cmd[2] == 'i') && (cmd[3] == 't')) {
		i = 4;
		while (cmd[i] == ' ')
			i++;
		if ((cmd[i] != '\n') && (i == 4)) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return;
		}
		exit(0);

	// View command
	} else if ((cmd[0] == 'v') && (cmd[1] == 'i') &&
			(cmd[2] == 'e') && (cmd[3] == 'w')) {
		char address1[4];
		char address2[4];
		// Move to arguement and check that it is present
		i = 4;
		while (cmd[i] == ' ')
			i++;
		// Check for missing address
		if (cmd[i] == '\n') {
			printf("  %s Address:%s Formats 'all', [address], or %s\n",
					E_MISSING_, C_NONE, "[address]-[address]");
			return;
		}
		// Check spaces follow 'view'
		if (i == 4) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return;
		}
		// Check for view all
		if ((cmd[i] == 'a') && (cmd[i+1] == 'l') && (cmd[i+2] == 'l')) {
			i += 3;
			while (cmd[i] == ' ')
				i++;
			// Check additional arguement are not provided
			if (cmd[i] != '\n') {
				printf("  %s Address:%s Formats 'all', [address], or %s\n",
						E_INVALID_, C_NONE, "[address]-[address]");
				return;
			}
			// Find last non-zero memory value
			unsigned short j = MEM_SIZE - 1;
			while (MEM[j] == 0)
				j--;
			j++;
			// Print memory
			unsigned char width = 0;
			unsigned char width_max = 8;
			char *w = getenv("WIDTH");
			if (w != NULL)
				width_max = strtol(w, NULL, 10)/11;
			for (int k = 0; k <= MEM_SIZE; k++) {
				// Check for line break
				if (width + 1 > width_max) {
					if (k > j)
						break;
					printf("\n");
					width = 0;
				}
				printf("  [%03x]%04x", k, MEM[k]);
				width += 1;
			}
			printf("\n");
			return;
		}
		// Read first address
		int j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 3) {
				printf("  %s Address:%s Too long\n", E_INVALID_, C_NONE);
				return;
			}
			address1[j] = cmd[i];
			j++;
			i++;
		}
		address1[j] = '\0';
		while (cmd[i] == ' ')
			i++;
		// Print value if only one address is present
		if (cmd[i] == '\n') {
			printf("  [%03x]%04x\n", (unsigned short)strtol(address1, NULL, 16),
					MEM[strtol(address1, NULL, 16)]);
			return;
		}
		// Check a second address is not provided without a dash
		if (isxdigit(cmd[i])) {
			printf("  %s Format:%s To enter a range use [address]-[address]\n",
					E_INVALID_, C_NONE);
			return;
		}
		// Check if first address is not all hex
		if ((j == 0) || (cmd[i] != '-')) {
			printf("  %s Address:%s Must be hexadecimal\n", E_INVALID_, C_NONE);
			return;
		}
		// Move to second address
		i++;
		while (cmd[i] == ' ')
			i++;
		// Read second address
		j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 3) {
				printf("  %s Address:%s Too long\n", E_INVALID_, C_NONE);
				return;
			}
			address2[j] = cmd[i];
			j++;
			i++;
		}
		while (cmd[i] == ' ')
			i++;
		// Check if second address is not all hex
		if ((j == 0) || (cmd[i] != '\n')) {
			printf("  %s Address:%s Must be hexadecimal\n", E_INVALID_, C_NONE);
			return;
		}
		address2[j] = '\0';
		unsigned short a1 = strtol(address1, NULL, 16);
		unsigned short a2 = strtol(address2, NULL, 16);
		if (a2 > a1) {
			// Print values in range
			unsigned char width = 0;
			unsigned char width_max = 8;
			char *w = getenv("WIDTH");
			if (w != NULL)
				width_max = strtol(w, NULL, 10)/11;
			for (int k = a1; k <= a2; k++) {
				// Check for line break
				if (width + 1 > width_max) {
					printf("\n");
					width = 0;
				}
				printf("  [%03x]%04x", k, MEM[k]);
				width += 1;
			}
			printf("\n");
			return;
		} else {
			printf("  %s Range%s\n", E_INVALID_, C_NONE);
			return;
		}

	// Set command
	} else if ((cmd[0] == 's') && (cmd[1] == 'e') &&
			(cmd[2] == 't')) {
		char address[4];
		char value[5];
		// Move to arguements and check that they are present
		i = 3;
		while (cmd[i] == ' ')
			i++;
		// Check for missing address
		if (cmd[i] == '\n') {
			printf("  %s Address:%s Format [address] [value]\n",
					E_MISSING_, C_NONE);
			return;
		}
		if (i == 3) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return;
		}
		// Read first arguement (address)
		int j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 3) {
				printf("  %s Address:%s Too long\n", E_INVALID_, C_NONE);
				return;
			}
			address[j] = cmd[i];
			j++;
			i++;
		}
		// Check if first arguement is not all hex
		if ((j == 0) || ((cmd[i] != ' ') && (cmd[i] != '\n'))) {
			printf("  %s Address:%s Must be hexadecimal\n", E_INVALID_, C_NONE);
			return;
		}
		address[j] = '\0';
		while (cmd[i] == ' ')
			i++;
		// Check second arguement is present
		if (cmd[i] == '\n') {
			printf("  %s Value:%s Format [address] [value]\n", E_MISSING_, C_NONE);
			return;
		}
		// Read second arguement (value)
		j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 4) {
				printf("  %s Value:%s Too long\n", E_INVALID_, C_NONE);
				return;
			}
			value[j] = cmd[i];
			j++;
			i++;
		}
		while (cmd[i] == ' ')
			i++;
		// Check if second arguement is not all hex
		if ((j == 0) || (cmd[i] != '\n')) {
			printf("  %s Address:%s Must be hexadecimal\n", E_INVALID_, C_NONE);
			return;
		}
		value[j] = '\0';
		// Set memory value
		MEM[strtol(address, NULL, 16)] = strtol(value, NULL, 16);
	} else {
		printf("  %s\n", E_UNKNOWN_CMD);
	}
}

int main(int argc, char **argv) {
	// Setup memory
	for (int i = 0; i < MEM_SIZE; i++)
		MEM[i] = 0;
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
				add_breakpoint(0);
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
							add_breakpoint(strtol(num, NULL, 16));
							k = 0;
						} else {
							num[k] = argv[i][j];
							k++;
						}
					}
				} else {
					add_breakpoint(strtol(argv[i], NULL, 16));
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
			MEM[num] = strtol(hex, NULL, 16);
			num++;
			i = 0;
		} else {
			line[i++] = ch;
		}
	}

	// Run program
	while (1) {
		// Check breakpoints
		if (is_breakpoint(PC))
			DEBUG_STEP = 4;
		// Get debug command if necessary
		if (DEBUG_STEP) {
			printf("%sdebug[%s%03x%s]:%s ", C_BLUE, C_NONE, PC, C_BLUE, C_NONE);
			char input[64];
			fgets(input, 64, stdin);
			if (feof(stdin)) {
				printf("exit\n");
				return 0;
			}
			debug(input);
		// Otherwise continue program
		} else {
			step(OPT_VOLUME);
		}
	}
	return 0;
}
