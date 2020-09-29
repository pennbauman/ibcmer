// IBCMer - IBCM Executor
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
#include "executor.h"

// Execute one command and progess the program
void step(ibcmemory *data, int volume) {
	unsigned short temp = 0;
	// Check for memory overflow
	if (data->pc == MEM_SIZE) {
		printf("%s Program overran memory\n", E_ERROR);
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
				printf("i/o\n");
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
								data->acc = strtol(input, NULL, 16);
								if (volume > 1)
									printf("  [ACC]%04x\n", data->acc);
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
							data->acc = input[0];
							if (volume > 1)
								printf("  [ACC]%04x\n", data->acc);
							break;
						}
					}
					break;
				// Hex ouput
				case 8:
					if (volume > 1)
						printf("Output: ");
					printf("%04x", data->acc);
					if (volume > 0)
						printf("\n");
					break;
				// Char ouput
				case 12:
					if (volume > 1)
						printf("Output: ");
					printf("%c", data->acc);
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
						printf("[ACC]%04x = [ACC]%04x << %x\n",
								temp, data->acc, distance);
					break;
				// Shift right, insert 0s
				case 4:
					temp = data->acc >> distance;
					if (volume > 1)
						printf("[ACC]%04x = [ACC]%04x >> %x\n",
								temp, data->acc, distance);
					break;
				// Rotate left, wrap
				case 8:
					temp = (data->acc << distance) + (data->acc >> (16 - distance));
					if (volume > 1)
						printf("[ACC]%04x = [ACC]%04x <= %x\n",
								temp, data->acc, distance);
					break;
				// Rotate right, wrap
				case 12:
					temp = (data->acc >> distance) + (data->acc << (16 - distance));
					if (volume > 1)
						printf("[ACC]%04x = [ACC]%04x => %x\n",
								temp, data->acc, distance);
					break;
				// Check invalid subcommands
				default:
					printf("\n%s Invalid shift operation code\n", E_ERROR);
					exit(1);
			}
			data->acc = temp;
			break;
		// Load value
		case 3:
			if (volume > 1)
				printf("load  [ACC]%04x\n", data->acc);
			data->acc = data->mem[address];
			break;
		// Store value
		case 4:
			if (volume > 1)
				printf("store [%03x]%04x\n", address, data->acc);
			data->mem[address] = data->acc;
			break;
		// Add
		case 5:
			temp = data->acc + data->mem[address];
			if (volume > 1)
				printf("add   [ACC]%04x = [ACC]%04x + [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// Subtract
		case 6:
			temp = data->acc - data->mem[address];
			if (volume > 1)
				printf("sub   [ACC]%04x = [ACC]%04x - [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// AND
		case 7:
			temp = data->acc & data->mem[address];
			if (volume > 1)
				printf("and   [ACC]%04x = [ACC]%04x & [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// OR
		case 8:
			temp = data->acc | data->mem[address];
			if (volume > 1)
				printf("or    [ACC]%04x = [ACC]%04x | [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// XOR
		case 9:
			temp = data->acc ^ data->mem[address];
			if (volume > 1)
				printf("xor   [ACC]%04x = [ACC]%04x ^ [%03x]%04x\n",
						temp, data->acc, address, data->mem[address]);
			data->acc = temp;
			break;
		// NOT
		case 10:
			temp = ~data->acc;
			if (volume > 1) {
				printf("not   [ACC]%04x = ~[ACC]%04x\n", temp, data->acc);
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
					printf("[ACC]%04x\n", data->acc);
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
					printf("[ACC]%04x\n", data->acc);
			}
			break;
		// Branch & Link
		case 15:
			if (volume > 1)
				printf("brl   [%03x]  [ACC]%04x\n", address, data->pc + 1);
			data->acc = data->pc + 1;
			data->pc = address - 1;
			break;
	}
	data->pc++;
}
