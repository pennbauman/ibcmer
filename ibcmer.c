// IBCMer   Penn Bauman
//   pennbauman@protonmail.com
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "text.h"

#define MEM_SIZE 4096
#define VERSION "0.3"

// Global variables
unsigned short ACC = 0;
unsigned short PC = 0;
unsigned short MEM[MEM_SIZE];
// Options
unsigned char OPT_CHECK = 0;
unsigned char OPT_VOLUME = 1;
// Debug variables
unsigned char DEBUG_STEP = 0;
unsigned short *DEBUG_BREAKPOINTS;
unsigned short DEBUG_BREAKS_COUNT = 0;
unsigned short DEBUG_BREAKS_MAX = 0;



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
	if (is_breakpoint(num))
		return;
	if (DEBUG_BREAKS_MAX == 0) {
		DEBUG_BREAKPOINTS = malloc(sizeof(short)*16);
		DEBUG_BREAKS_MAX = 16;
	}
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
	DEBUG_BREAKPOINTS[DEBUG_BREAKS_COUNT-1] = num;
	//printf("b: %d %d\n", DEBUG_BREAKS_COUNT-1, num);
}

// Check line numbers
signed char check_line_num(char* line, unsigned short line_num) {
	int i = 4;
	if ((line[i] != ' ') && (line[i] != '\t'))
		return 1;
	while ((line[i] == ' ') || (line[i] == '\t')) {
		i++;
	}
	int j = 0;
	char num[4];
	while ((line[i+j] != ' ') && (line[i+j] != '\t')) {
		if (! isxdigit(line[i + j]))
			return 2;
		if (j > 3)
			return 3;
		num[j] = line[i + j];
		j++;
	}
	num[j] = '\0';
	if (line_num != strtol(num, NULL, 16))
		return 4;
	return 0;
}

// Execute one command and progess the program
void step(int volume) {
	if (PC == MEM_SIZE) {
		printf("%sError:%s Program overran memory\n", C_RED, C_NONE);
		exit(1);
	}
	if (volume > 0)
		printf("[%03x]%04x  ", PC, MEM[PC]);
	unsigned short address = MEM[PC] << 4;
	address = address >> 4;
	switch (MEM[PC] >> 12) {
		case 0:
			if (volume > 0)
				printf("halt\n");
			exit(0);
		case 1:
			if (volume > 0)
				printf("i/o\n");
			unsigned char type = MEM[PC] >> 8;
			type = type << 4;
			type = type >> 4;
			char input[64];
			switch (type) {
				case 0:
					while (1) {
						printf("Input hex: ");
						fgets(input, 64, stdin);
						if (strlen(input) > 5) {
							printf("  Invalid hex input, too long\n");
						} else {
							char valid = 1;
							for (int i = 0; i < strlen(input) - 1; i++) {
								if (! isxdigit(input[i]))
									valid = 0;
							}
							if (valid) {
								ACC = strtol(input, NULL, 16);
								if (volume > 0)
									printf("  [ACC]%04x\n", ACC);
								break;
							} else {
								printf("  Invalid hex input\n");
							}
						}
					}
					break;
				case 4:
					while (1) {
						printf("Input char: ");
						fgets(input, 64, stdin);
						if (strlen(input) > 2) {
							printf("  Invalid char input, multiple characters\n");
						} else {
							ACC = input[0];
							if (volume > 0)
								printf("  [ACC]%04x\n", ACC);
							break;
						}
					}
					break;
				case 8:
					if (volume > 1)
						printf("Output: ");
					printf("%04x\n", ACC);
					break;
				case 12:
					if (volume > 1)
						printf("Output: ");
					printf("%c\n", ACC);
					break;
				default:
					printf("\n%sError:%s Invalid I/O operation code\n",
							C_RED, C_NONE);
					exit(1);
			}
			break;
		case 2:
			if (volume > 0)
				printf("shift ");
			unsigned char direction = MEM[PC] >> 8;
			direction = direction << 4;
			direction = direction >> 4;
			unsigned char distance = MEM[PC] << 4;
			distance = distance >> 4;
			unsigned short temp = 0;
			switch (direction) {
				case 0:
					temp = ACC << distance;
					if (volume > 0)
						printf("[ACC]%04x = [ACC]%04x << %x\n",
								temp, ACC, distance);
					break;
				case 4:
					temp = ACC >> distance;
					if (volume > 0)
						printf("[ACC]%04x = [ACC]%04x >> %x\n",
								temp, ACC, distance);
					break;
				case 8:
					temp = (ACC << distance) + (ACC >> (16 - distance));
					if (volume > 0)
						printf("[ACC]%04x = [ACC]%04x <= %x\n",
								temp, ACC, distance);
					break;
				case 12:
					temp = (ACC >> distance) + (ACC << (16 - distance));
					if (volume > 0)
						printf("[ACC]%04x = [ACC]%04x => %x\n",
								temp, ACC, distance);
					break;
				default:
					printf("\n%sError:%s Invalid shift operation code\n",
							C_RED, C_NONE);
					exit(1);
			}
			ACC = temp;
			break;
		case 3:
			if (volume > 0)
				printf("load  [ACC]%04x\n", ACC);
			ACC = MEM[address];
			break;
		case 4:
			if (volume > 0)
				printf("store [%03x]%04x\n", address, ACC);
			MEM[address] = ACC;
			break;
		case 5:
			if (volume > 0)
				printf("add   [ACC]%04x = [ACC]%04x + [%03x]%04x\n",
						ACC + MEM[address], ACC, address, MEM[address]);
			ACC += MEM[address];
			break;
		case 6:
			if (volume > 0)
				printf("sub   [ACC]%04x = [ACC]%04x - [%03x]%04x\n",
						ACC - MEM[address], ACC, address, MEM[address]);
			ACC -= MEM[address];
			break;
		case 7:
			if (volume > 0)
				printf("and   [ACC]%04x = [ACC]%04x & [%03x]%04x\n",
						ACC & MEM[address], ACC, address, MEM[address]);
			ACC &= MEM[address];
			break;
		case 8:
			if (volume > 0)
				printf("or    [ACC]%04x = [ACC]%04x | [%03x]%04x\n",
						ACC | MEM[address], ACC, address, MEM[address]);
			ACC |= MEM[address];
			break;
		case 9:
			if (volume > 0)
				printf("xor   [ACC]%04x = [ACC]%04x ^ [%03x]%04x\n",
						ACC ^ MEM[address], ACC, address, MEM[address]);
			ACC ^= MEM[address];
			break;
		case 10:
			if (volume > 0) {
				unsigned short temp = ~ACC;
				printf("not   [ACC]%04x = ~ [ACC]%04x\n", temp, ACC);
			}
			ACC = ~ACC;
			break;
		case 11:
			if (volume > 0)
				printf("nop\n");
			break;
		case 12:
			if (volume > 0)
				printf("jmp   [%03x]\n", address);
			PC = address - 1;
			break;
		case 13:
			if (volume > 0)
				printf("jmpe  ");
			if (ACC == 0) {
				if (volume > 0)
					printf("[%03x]\n", address);
				PC = address - 1;
			} else {
				if (volume > 0)
					printf("[ACC]%04x\n", ACC);
			}
			break;
		case 14:
			if (volume > 0)
				printf("jmpl  ");
			if (ACC >> 15 == 1) {
				if (volume > 0)
					printf("[%03x]\n", address);
				PC = address - 1;
			} else {
				if (volume > 0)
					printf("[ACC]%04x\n", ACC);
			}
			break;
		case 15:
			if (volume > 0)
				printf("brl   [%03x]  [ACC]%04x\n", address, PC + 1);
			ACC = PC + 1;
			PC = address - 1;
			break;
	}
	PC++;
}

// debug commands
void debug(char *cmd) {
	// Step command
	if ((0 == strcmp("\n", cmd)) || (0 == strcmp("step\n", cmd))) {
		step(1);
	// Run command
	} else if (0 == strcmp("run\n", cmd)) {
		DEBUG_STEP = 0;
		step(1);
	// Exit command
	} else if (0 == strcmp("exit\n", cmd)) {
		exit(0);

	// View command
	} else if ((cmd[0] == 'v') && (cmd[1] == 'i') &&
			(cmd[2] == 'e') && (cmd[3] == 'w')) {
		char address1[4];
		char address2[4];
		// Move to arguement and check that it is present
		int i = 4;
		while (cmd[i] == ' ')
			i++;
		// Check for view all
		if ((cmd[i] == 'a') && (cmd[i+1] == 'l') && (cmd[i+2] == 'l')) {
			i += 3;
			while (cmd[i] == ' ')
				i++;
			// Check additional arguement are not provided
			if (cmd[i] != '\n') {
				printf("  Invalid address\n");
				printf("   Usage: view [all|address|address-address]\n");
				return;
			}
			// Find last non-zero memory value
			unsigned short j = MEM_SIZE - 1;
			while (MEM[j] == 0)
				j--;
			j += 2;
			// Print memory
			unsigned char width = 0;
			unsigned char width_max = 8;
			char *w = getenv("WIDTH");
			if (w != NULL)
				width_max = strtol(w, NULL, 10)/11;
			for (int k = 0; k <= j; k++) {
				if (width + 1 > width_max) {
					printf("\n");
					width = 0;
				}
				printf("  [%03x]%04x", k, MEM[k]);
				width += 1;
			}
			printf("\n");
			return;
		}
		if (cmd[i] == '\n') {
			printf("  Missing address\n");
			printf("   Usage: view [all|address|address-address]\n");
			return;
		}
		if (i == 4) {
			printf("  Unknown command\n");
			return;
		}
		// Read first address
		int j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 3) {
				printf("  Invalid address, too long\n");
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
			printf("  Invalid format\n   To enter a range use 'address-address'\n");
			return;
		}
		// Check if first address is not all hex
		if ((j == 0) || (cmd[i] != '-')) {
			printf("  Invalid address, must be hexadecimal\n");
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
				printf("  Invalid address, too long\n");
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
			printf("  Invalid address, must be hexadecimal\n");
			return;
		}
		address2[j] = '\0';
		unsigned short a1 = strtol(address1, NULL, 16);
		unsigned short a2 = strtol(address2, NULL, 16);
		if (a2 > a1) {
			unsigned char width = 0;
			unsigned char width_max = 8;
			char *w = getenv("WIDTH");
			if (w != NULL)
				width_max = strtol(w, NULL, 10)/11;
			for (int k = a1; k <= a2; k++) {
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
			printf("  Invalid range\n");
			return;
		}

	// Set command
	} else if ((cmd[0] == 's') && (cmd[1] == 'e') &&
			(cmd[2] == 't')) {
		char address[4];
		char value[5];
		// Move to arguements and check that they are present
		int i = 3;
		while (cmd[i] == ' ')
			i++;
		if (cmd[i] == '\n') {
			printf("  Missing address\n   Usage: set [address] [value]\n");
			return;
		}
		if (i == 3) {
			printf("  Unknown command\n");
			return;
		}
		// Read first arguement (address)
		int j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 3) {
				printf("  Invalid address, too long\n");
				return;
			}
			address[j] = cmd[i];
			j++;
			i++;
		}
		// Check if first arguement is not all hex
		if ((j == 0) || (cmd[i] != ' ')) {
			printf("  Invalid address, must be hexadecimal\n");
			return;
		}
		address[j] = '\0';
		while (cmd[i] == ' ')
			i++;
		// Check second arguement is present
		if (cmd[i] == '\n') {
			printf("  Missing value\n   Usage: set [address] [value]\n");
			return;
		}
		// Read second arguement (value)
		j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 4) {
				printf("  Invalid value, too long\n");
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
			printf("  Invalid value, must be hexadecimal\n");
			return;
		}
		value[j] = '\0';
		// Set memory value
		MEM[strtol(address, NULL, 16)] = strtol(value, NULL, 16);
	} else {
		printf("  Unknown command\n");
	}
}

int main(int argc, char **argv) {
	// Setup memory
	for (int i = 0; i < MEM_SIZE; i++) {
		MEM[i] = 0;
	}

	// Read through command line arguements
	char* src_path = "";
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if (0 == strcmp("--version", argv[i])) {
				printf("IBCMer v%s\n", VERSION);
				return 0;
			} else if ((0 == strcmp("--help", argv[i])) ||
					(0 == strcmp("-h", argv[i]))) {
				printf("%s\n", HELP);
				return 0;
			} else if (0 == strcmp("--help-ibcm", argv[i])) {
				printf("%s\n", HELP_IBCM);
				return 0;
			} else if ((0 == strcmp("--check", argv[i])) ||
					(0 == strcmp("-c", argv[i]))) {
				OPT_CHECK = 1;
			} else if ((0 == strcmp("--quiet", argv[i])) ||
					(0 == strcmp("-q", argv[i]))) {
				OPT_VOLUME = 0;
			} else if ((0 == strcmp("--step", argv[i])) ||
					(0 == strcmp("-s", argv[i]))) {
				add_breakpoint(0);
			} else if ((0 == strcmp("--break", argv[i])) ||
					(0 == strcmp("-b", argv[i]))) {
				i++;
				signed char is_num = 0;
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
				if (is_num == 2) {
					printf("%sError:%s Invalid breakpoint '%s'\n",
							C_RED, C_NONE, argv[i]);
					return 1;
				} else if (is_num == 1) {
					char num[4];
					int k = 0;
					for (int j = 0; j < strlen(argv[i])+1; j++) {
						if (! isxdigit(argv[i][j])) {
							if (k == 0) {
								printf("%sError:%s Invalid breakpoints '%s'\n",
										C_RED, C_NONE, argv[i]);
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
				printf("%sError:%s Unknown options '%s'\n", C_RED, C_NONE, argv[i]);
				return 1;
			}
		} else {
			if (src_path[0] == '\0') {
				src_path = argv[i];
			} else {
				printf("%sError:%s Multiple files provided, only one allowed\n",
						C_RED, C_NONE);
				return 1;
			}
		}
	}

	// Open code file
	if (strlen(src_path) == 0) {
		printf("%sError:%s A code file must be provided\n", C_RED, C_NONE);
		return 1;
	}
	FILE *src = fopen(src_path, "r");
	if (src == NULL) {
		printf("%sError:%s Code file '%s' not found\n", C_RED, C_NONE, src_path);
		return 1;
	}
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
					printf("%sError:%s Invalid operation code on line %d of '%s'\n",
							C_RED, C_NONE, num + 1, src_path);
					printf("\n| %s%s%s\n\n", C_YELLOW, line, C_NONE);
					return 1;
				}
			}
			// Check for invalid line number
			if (OPT_CHECK) {
				if (check_line_num(line, num)) {
					printf("%sError:%s Improper line number on line %d of '%s'\n",
							C_RED, C_NONE, num + 1, src_path);
					printf("\n| %s%s%s\n\n", C_YELLOW, line, C_NONE);
					return 1;
				}
			}
			//printf("%s\n  %d\n", line, check);
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

	for (int i = 0; i < DEBUG_BREAKS_COUNT; i++) {
		printf("b: %x\n", DEBUG_BREAKPOINTS[i]);
	}

	// Run program
	while (1) {
		if (is_breakpoint(PC)) {
			DEBUG_STEP = 4;
		}
		if (DEBUG_STEP) {
			printf("[%03x]: ", PC);
			char input[64];
			fgets(input, 64, stdin);
			debug(input);
		} else {
			step(OPT_VOLUME);
		}
	}
	return 0;
}
