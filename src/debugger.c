// IBCMer - Debugging Tools
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
#include "debugger.h"


// Split string
char** split_str(char* src) {
	int num_words = 0;
	int i = 0;
	int state = 0;
	while (src[i] != 0) {
		if (src[i] == '\n')
			break;
		if ((src[i] == ' ') || (src[i] == '\t')) {
			state = 0;
		} else {
			if (state == 0)
				num_words++;
			state = 1;
		}
		i++;
	}
	//printf("num_words = %d\n", num_words);
	char** fin = malloc((1+num_words) * sizeof(char*));
	fin[0] = malloc((1+num_words) * sizeof(char));
	fin[0][0] = num_words;
	//printf("fin[0][0] = %d\n", fin[0][0]);
	i = 0;
	for (int w = 1; w <= num_words; w++) {
		while ((src[i] == ' ') || (src[i] == '\t'))
			i++;
		fin[0][w] = i;
		//printf("fin[0][%d] = %d\n", w, fin[0][w]);
		int l = 0;
		while ((src[i+l] != ' ') && (src[i+l] != '\t') && (src[i+l] != 0))
			l++;
		fin[w] = malloc((l+1) * sizeof(char));
		for (int k = 0; k < l; k++) {
			//printf("word[%d] = '%c'\n", k, src[i+k]);
			fin[w][k] = src[i+k];
		}
		fin[w][l] = 0;
		//printf("w[%d] = '%s'\n", w, fin[w]);
		i += l;
	}
	return fin;
}


// Initialize breakpoints
breakpoints init_breakpoints() {
	breakpoints breaks;
	breaks.array = malloc(sizeof(short)*16);
	breaks.max = 16;
	breaks.count = 0;
	return breaks;
}

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
	char** parsed_cmd = split_str(cmd);

	// Step command
	if (parsed_cmd[0][0] == 0) {
		step(data, 2);
		return 1;
	}
	if (0 == strcmp(parsed_cmd[1], "step")) {
		if (parsed_cmd[0][0] == 1) {
			step(data, 2);
			return 1;
		} else if (parsed_cmd[0][0] == 2) {
			// Check number length
			unsigned char i = 0;
			while (isdigit(parsed_cmd[2][i])) {
				if (i == 3) {
					printf("  %s Number:%s Too long\n", E_INVALID_, C_NONE);
					return 1;
				}
				i++;
			}
			// Check valid number
			if (parsed_cmd[2][i] != 0) {
				if (isxdigit(parsed_cmd[2][i])) {
					printf("  %s Number:%s Must be decimal\n", E_INVALID_, C_NONE);
				} else {
					printf("  %s Number%s\n", E_INVALID_, C_NONE);
				}
				return 1;
			}
			// Step program
			unsigned short num = strtol(parsed_cmd[2], NULL, 10);
			for (i = 0; i < num; i++)
				step(data, 2);
			return 1;
		} else {
			printf("  %s Number:%s Too many provided\n", E_INVALID_, C_NONE);
			return 1;
		}
	}

	// Run command
	if (0 == strcmp(parsed_cmd[1], "run")) {
		step(data, volume);
		return 0;
	}

	// Unknown command
	printf("  %s\n", E_UNKNOWN_CMD);
	return 1;
}
