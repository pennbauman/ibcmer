// IBCMer
//   Penn Bauman
//   pennbauman@protonmail.com
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MEM_SIZE 4096

// Global variables
unsigned short ACC = 0;
unsigned short PC = 0;
unsigned short MEM[MEM_SIZE];
// Options
unsigned char OPT_CHECK = 0;
unsigned char OPT_VOLUME = 1;
unsigned char OPT_STEP = 0;

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
		printf("Error: Program overran memory\n");
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
						if (strlen(input) > 1) {
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
					printf("Output: %04x\n", ACC);
					break;
				case 12:
					printf("Output: %c\n", ACC);
					break;
				default:
					printf("\nError: Invalid I/O operation code\n");
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

int main(int argc, char **argv) {
	// Setup memory
	for (int i = 0; i < MEM_SIZE; i++) {
		MEM[i] = 0;
	}

	// Read through command line arguements
	char* src_path = "";
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			if ((0 == strcmp("--check", argv[i])) || (0 == strcmp("-c", argv[i]))) {
				OPT_CHECK = 1;
			} else if ((0 == strcmp("--quiet", argv[i])) || (0 == strcmp("-q", argv[i]))) {
				OPT_VOLUME = 0;
			} else {
				printf("Error: Unkown options '%s'\n", argv[i]);
				return 1;
			}
		} else {
			if (src_path[0] == '\0') {
				src_path = argv[i];
			} else {
				printf("Error: Multiple files provided, only one allowed\n");
				return 1;
			}
		}
	}

	// Open code file
	if (strlen(src_path) == 0) {
		printf("Error: A code file must be provided\n");
		return 1;
	}
	FILE *src = fopen(src_path, "r");
	if (src == NULL) {
		printf("Error: Code file '%s' not found\n", src_path);
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
					printf("Error: Invalid operation code on line %d of '%s'\n",
							num + 1, src_path);
					printf("\n|  %s\n\n", line);
					return 1;
				}
			}
			// Check for invalid line number
			if (OPT_CHECK) {
				if (check_line_num(line, num)) {
					printf("%d\n", check_line_num(line, num));
					printf("Error: Improper line number on line %d of '%s'\n",
							num + 1, src_path);
					printf("\n|  %s\n\n", line);
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
	
	// Run program
	while (1) {
		step(OPT_VOLUME);
	}
	return 0;
}
