// IBCM Executor - IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
#include "executor.h"

// Check line numbers are properly formatted
signed char check_line_num(char* line, unsigned short line_num) {
	int i = 4;
	// Check there are spaces after the command
	if (!isspace(line[i]))
		return i;
	// Skip over whitespace
	while (isspace(line[i]))
		i++;
	//Parse number
	int j = 0;
	char num[4];
	while (!isspace(line[i+j]) && (line[i+j] != 0)) {
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

// Step memory from file
unsigned char read_file(ibcmemory *data, char* filename, unsigned char num_check) {
	for (int i = 0; i < MEM_SIZE; i++)
		data->mem[i] = 0;
	data->acc = 0;
	data->pc = 0;

	// Open code file
	if (strlen(filename) == 0) {
		fprintf(stderr, "%s A code file must be provided\n", E_ERROR);
		return 1;
	}
	// Check code file exists
	FILE *src = fopen(filename, "r");
	if (src == NULL) {
		fprintf(stderr, "%s Code file '%s' not found\n", E_ERROR, filename);
		return 1;
	}

	// Read from code file
	char ch, line[256];
	unsigned int num = 0;
	int i = 0;
	while ((ch = fgetc(src)) != EOF) {
		if (ch == '\n') {
			if (num >= MEM_SIZE) {
				fprintf(stderr, "%s Code file overflows memory (%d lines max)\n", E_ERROR, MEM_SIZE);
				return 1;
			}
			line[i] = '\0';
			// Check validity of code line
			int j = 0;
			for (; j < 4; j++) {
				if (! isxdigit(line[j])) {
					fprintf(stderr, "%s '%s:%d:%d' Invalid opcode hexadecimal\n",
							E_ERROR, filename, num + 1, j + 1);
					fprintf(stderr, "\n    %s\n    ", line);
					for (int k = 0; k < j; k++)
						fprintf(stderr, " ");
					fprintf(stderr, "%s^%s\n", C_YELLOW, C_NONE);
					return 1;
				}
			}
			// Check for invalid line number
			if (num_check) {
				unsigned char test = check_line_num(line, num);
				if (test == 1) {
					fprintf(stderr, "%s '%s:%d' Incorrect line number\n",
							E_ERROR, filename, num + 1);
					fprintf(stderr, "\n    %s\n        ", line);
					int k = 4;
					while ((line[k] == ' ') || (line[k] == '\t')) {
						fprintf(stderr, " ");
						k++;
					}
					fprintf(stderr, "%s", C_YELLOW);
					while ((line[k] != ' ') && (line[k] != '\t')) {
						fprintf(stderr, "^");
						k++;
					}
					fprintf(stderr, "%s\n", C_NONE);
					return 1;

				}
				if (test > 2) {
					fprintf(stderr, "%s '%s:%d:%d' Invalid line number\n",
							E_ERROR, filename, num + 1, test + 1);
					fprintf(stderr, "\n    %s\n    ", line);
					for (int k = 0; k < test; k++)
						fprintf(stderr, " ");
					fprintf(stderr, "%s^%s\n", C_YELLOW, C_NONE);
					return 1;
				}
			}

			// Parse command and save it to memory
			line[4] = '\0';
			data->mem[num] = strtol(line, NULL, 16);
			num++;
			i = 0;
		} else {
			line[i++] = ch;
		}
	}
	return 0;
}



// Execute one command and progess the program
void step(ibcmemory *data, int volume) {
	unsigned short temp = 0;
	// Check for memory overflow
	if (data->pc == MEM_SIZE) {
		fprintf(stderr, "%s Memory overflow (PC = 0x%04x)\n", E_ERROR, data->pc);
		exit(1);
	}
	if (volume > 1)
		printf("[%03x]%04x  ", data->pc, data->mem[data->pc]);
	// Setup address value
	unsigned short address = data->mem[data->pc] << 4;
	address = address >> 4;
	// Check command
	switch (data->mem[data->pc] >> 12) {
		// Halt
		case 0:
			if (volume > 1)
				printf("halt\n");
			exit(0);
		// I/O
		case 1:
			if (volume > 1)
				printf("i/o   (ACC)%04x\n", data->acc);
			unsigned char type = data->mem[data->pc] >> 8;
			type = type << 4;
			type = type >> 4;
			char input[64];
			// Check I/O type
			switch (type) {
				// Hex input
				case 0:
					while (1) {
						if (volume > 0)
							printf("Input hex:  ");
						fgets(input, 64, stdin);
						// Check length is 4
						if (strlen(input) > 5) {
							fprintf(stderr, "  %s Too long\n", E_INVALID_IN);
						} else if (strlen(input) < 2) {
							fprintf(stderr, "  %s\n", E_MISSING_IN);
						} else {
							// Check all characters are valid hex
							char valid = 1;
							for (int i = 0; i < strlen(input) - 1; i++) {
								if (! isxdigit(input[i]))
									valid = 0;
							}
							// Read hex value
							if (valid) {
								data->acc = strtol(input, NULL, 16);
								break;
							} else {
								fprintf(stderr, "  %s Not hexadecimal\n", E_INVALID_IN);
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
							fprintf(stderr, "  %s Multiple characters\n", E_INVALID_IN);
						} else if (strlen(input) < 2) {
							fprintf(stderr, "  %s\n", E_MISSING_IN);
						} else {
							// Read char value
							data->acc = input[0];
							break;
						}
					}
					break;
				// Hex ouput
				case 8:
					if (volume > 1)
						printf("Output hex:  ");
					printf("%04x", data->acc);
					if (volume > 0)
						printf("\n");
					break;
				// Char ouput
				case 12:
					if (volume > 1)
						printf("Output char: ");
					printf("%c", data->acc);
					if (volume > 0)
						printf("\n");
					break;
				// Check invalid subcommands
				default:
					printf("\n");
					fprintf(stderr, "%s Invalid I/O sub-opcode '%x'\n", E_ERROR, type);
					exit(1);
			}
			break;
		// Shift
		case 2:
			if (volume > 1)
				printf("shift ");
			// Get direction value
			unsigned char direction = data->mem[data->pc] >> 8;
			direction = direction << 4;
			direction = direction >> 4;
			// Get distance value
			unsigned char distance = data->mem[data->pc] << 4;
			distance = distance >> 4;
			switch (direction) {
				// Shift left, insert 0s
				case 0:
					temp = data->acc << distance;
					if (volume > 1)
						printf("(ACC)%04x = (ACC)%04x << %x\n",
								temp, data->acc, distance);
					break;
				// Shift right, insert 0s
				case 4:
					temp = data->acc >> distance;
					if (volume > 1)
						printf("(ACC)%04x = (ACC)%04x >> %x\n",
								temp, data->acc, distance);
					break;
				// Rotate left, wrap
				case 8:
					temp = (data->acc << distance) + (data->acc >> (16 - distance));
					if (volume > 1)
						printf("(ACC)%04x = (ACC)%04x <= %x\n",
								temp, data->acc, distance);
					break;
				// Rotate right, wrap
				case 12:
					temp = (data->acc >> distance) + (data->acc << (16 - distance));
					if (volume > 1)
						printf("(ACC)%04x = (ACC)%04x => %x\n",
								temp, data->acc, distance);
					break;
				// Check invalid subcommands
				default:
					printf("\n");
					fprintf(stderr, "%s Invalid shift sub-opcode '%x'\n", E_ERROR, direction);
					exit(1);
			}
			data->acc = temp;
			break;
		// Load value
		case 3:
			data->acc = data->mem[address];
			if (volume > 1)
				printf("load  (ACC)%04x = [%03x]%04x\n", data->acc, address, data->acc);
			break;
		// Store value
		case 4:
			if (volume > 1)
				printf("store [%03x]%04x = (ACC)%04x\n", address, data->acc, data->acc);
			data->mem[address] = data->acc;
			break;
		// Add
		case 5:
			temp = data->acc + data->mem[address];
			if (volume > 1)
				printf("add   (ACC)%04x = (ACC)%04x + [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// Subtract
		case 6:
			temp = data->acc - data->mem[address];
			if (volume > 1)
				printf("sub   (ACC)%04x = (ACC)%04x - [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// AND
		case 7:
			temp = data->acc & data->mem[address];
			if (volume > 1)
				printf("and   (ACC)%04x = (ACC)%04x & [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// OR
		case 8:
			temp = data->acc | data->mem[address];
			if (volume > 1)
				printf("or    (ACC)%04x = (ACC)%04x | [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// XOR
		case 9:
			temp = data->acc ^ data->mem[address];
			if (volume > 1)
				printf("xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// NOT
		case 10:
			temp = ~data->acc;
			if (volume > 1) {
				printf("not   (ACC)%04x = ! (ACC)%04x\n", temp, data->acc);
			}
			data->acc = temp;
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
			data->pc = address - 1;
			break;
		// Jump, data->acc = 0
		case 13:
			if (volume > 1)
				printf("jmpe  ");
			if (data->acc == 0) {
				if (volume > 1)
					printf("[%03x]\n", address);
				data->pc = address - 1;
			} else {
				if (volume > 1)
					printf("(ACC)%04x\n", data->acc);
			}
			break;
		// Jump, data->acc < 0
		case 14:
			if (volume > 1)
				printf("jmpl  ");
			if (data->acc >> 15 == 1) {
				if (volume > 1)
					printf("[%03x]\n", address);
				data->pc = address - 1;
			} else {
				if (volume > 1)
					printf("(ACC)%04x\n", data->acc);
			}
			break;
		// Branch & Link
		case 15:
			if (volume > 1)
				printf("brl   [%03x]  (ACC)%04x\n", address, data->pc + 1);
			data->acc = data->pc + 1;
			data->pc = address - 1;
			break;
	}
	data->pc++;
}
