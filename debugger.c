// IBCMer - Debugging Tools
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
#include "debugger.h"

// Check if breakpoint exists
signed char is_breakpoint(breakpoints *breaks, unsigned short num) {
	for (int i = 0; i < breaks->count; i++) {
		if (breaks->array[i] == num)
			return 4;
	}
	return 0;
}

// Add new breakpoint
void add_breakpoint(breakpoints *breaks, unsigned short num) {
	// Skip duplicates
	if (is_breakpoint(breaks, num))
		return;
	// Create breakpoint array if necessary
	if (breaks->max == 0) {
		breaks->array = malloc(sizeof(short)*16);
		breaks->max = 16;
	}
	// Grows breakpoint vector possibly copying array
	breaks->count++;
	if (breaks->count == breaks->max) {
		unsigned short *temp = malloc(sizeof(short)*breaks->max*2);
		for (int i = 0; i < breaks->max; i++) {
			temp[i] = breaks->array[i];
		}
		breaks->max *= 2;
		free(breaks->array);
		breaks->array = temp;
	}
	// Writes new breakpoints
	breaks->array[breaks->count-1] = num;
}


// Run debug commands
unsigned char debug(ibcmemory *data, breakpoints *breaks, char *cmd, unsigned char volume) {
	unsigned short i = 0;
	while (cmd[i] == ' ')
		cmd = &cmd[1];
	// Step command
	if (0 == strcmp("\n", cmd)) {
		step(data, 2);
	} else if ((cmd[0] == 's') && (cmd[1] == 't') &&
			(cmd[2] == 'e') && (cmd[3] == 'p')) {
		i = 4;
		while (cmd[i] == ' ')
			i++;
		if ((cmd[i] != '\n') && (i == 4)) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return 1;
		}
		if (cmd[i] == '\n') {
			step(data, 2);
			return 1;
		}
		unsigned int j = 0;
		char numstr[4];
		while (isdigit(cmd[i])) {
			if (j == 3) {
				printf("  %s Number:%s Too long\n", E_INVALID_, C_NONE);
				return 1;
			}
			numstr[j] = cmd[i];
			j++;
			i++;
		}
		if ((cmd[i] != ' ') && (cmd[i] != '\n')) {
			if (isxdigit(cmd[i])) {
				printf("  %s Number:%s Must be decimal\n", E_INVALID_, C_NONE);
			} else {
				printf("  %s Number%s\n", E_INVALID_, C_NONE);
			}
			return 1;
		}
		while (cmd[i] == ' ')
			i++;
		if (cmd[i] != '\n') {
			printf("  %s Number:%s Too many provided\n", E_INVALID_, C_NONE);
			return 1;
		}
		unsigned short num = strtol(numstr, NULL, 10);
		for (j = 0; j < num; j++)
			step(data, 2);
		return 1;

	// Run command
	} else if ((cmd[0] == 'r') && (cmd[1] == 'u') &&
			(cmd[2] == 'n')) {
		i = 3;
		while (cmd[i] == ' ')
			i++;
		if ((cmd[i] != '\n') && (i == 3)) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return 1;
		}
		step(data, volume);
		return 0;

	// Exit command
	} else if ((cmd[0] == 'e') && (cmd[1] == 'x') &&
			(cmd[2] == 'i') && (cmd[3] == 't')) {
		i = 4;
		while (cmd[i] == ' ')
			i++;
		if ((cmd[i] != '\n') && (i == 4)) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return 1;
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
			return 1;
		}
		// Check spaces follow 'view'
		if (i == 4) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return 1;
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
				return 1;
			}
			// Find last non-zero memory value
			unsigned short j = MEM_SIZE - 1;
			while (data->mem[j] == 0)
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
				printf("  [%03x]%04x", k, data->mem[k]);
				width += 1;
			}
			printf("\n");
			return 1;
		}
		// Read first address
		int j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 3) {
				printf("  %s Address:%s Too long\n", E_INVALID_, C_NONE);
				return 1;
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
					data->mem[strtol(address1, NULL, 16)]);
			return 1;
		}
		// Check a second address is not provided without a dash
		if (isxdigit(cmd[i])) {
			printf("  %s Format:%s To enter a range use [address]-[address]\n",
					E_INVALID_, C_NONE);
			return 1;
		}
		// Check if first address is not all hex
		if ((j == 0) || (cmd[i] != '-')) {
			printf("  %s Address:%s Must be hexadecimal\n", E_INVALID_, C_NONE);
			return 1;
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
				return 1;
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
			return 1;
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
				printf("  [%03x]%04x", k, data->mem[k]);
				width += 1;
			}
			printf("\n");
			return 1;
		} else {
			printf("  %s Range%s\n", E_INVALID_, C_NONE);
			return 1;
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
			return 1;
		}
		if (i == 3) {
			printf("  %s\n", E_UNKNOWN_CMD);
			return 1;
		}
		// Read first arguement (address)
		int j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 3) {
				printf("  %s Address:%s Too long\n", E_INVALID_, C_NONE);
				return 1;
			}
			address[j] = cmd[i];
			j++;
			i++;
		}
		// Check if first arguement is not all hex
		if ((j == 0) || ((cmd[i] != ' ') && (cmd[i] != '\n'))) {
			printf("  %s Address:%s Must be hexadecimal\n", E_INVALID_, C_NONE);
			return 1;
		}
		address[j] = '\0';
		while (cmd[i] == ' ')
			i++;
		// Check second arguement is present
		if (cmd[i] == '\n') {
			printf("  %s Value:%s Format [address] [value]\n", E_MISSING_, C_NONE);
			return 1;
		}
		// Read second arguement (value)
		j = 0;
		while (isxdigit(cmd[i])) {
			if (j == 4) {
				printf("  %s Value:%s Too long\n", E_INVALID_, C_NONE);
				return 1;
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
		}
		value[j] = '\0';
		// Set memory value
		data->mem[strtol(address, NULL, 16)] = strtol(value, NULL, 16);
	} else {
		printf("  %s\n", E_UNKNOWN_CMD);
	}
	return 1;
}
