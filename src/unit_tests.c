// Unit Tests - IBCMer
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
#include <stdio.h>

#include "executor.h" // IBCM executor
#include "debugger.h" // debugging tools


unsigned int PASSED = 0;
unsigned int FAILED = 0;

void check(unsigned char condition, char *error) {
	if (condition) {
		PASSED++;
		return;
	} else {
		FAILED++;
		printf("\n  %s\n", error);
	}

}

int main() {
	printf("\33[32mUNIT TESTS\33[0m\n");
	printf("-------------------------------------------------------------\n");
	
	char error[128];
	// TEST BREAKPOINTS
	{
		breakpoints breaks = init_breakpoints();
		check((breaks.max == 16), "Breakpoints array not initialized properly");

		// Add breakpoints
		unsigned short points[] = {1, 42, 795, 4093};
		for (int i = 0; i < 4; i++) {
			add_breakpoint(&breaks, points[i]);
		}


		for (int i = 0; i < 4; i++) {
			sprintf(error, "Added breakpoint not found '%d'", i);
			check(is_breakpoint(&breaks, points[i]), error);
		}

		unsigned short points_absent[] = {8, -1, 203, 4094};
		for (int i = 0; i < 4; i++) {
			sprintf(error, "unadded breakpoint found '%d'", i);
			check(!is_breakpoint(&breaks, points_absent[i]), error);
		}

		check((breaks.count == 4), "Breakpoints count not set properly");

		add_breakpoint(&breaks, 12);
		check((breaks.count == 5), "Breakpoints count not increasing properly");
		add_breakpoint(&breaks, 1);
		check((breaks.count == 5), "Breakpoints inserting duplicate points");

		add_breakpoint(&breaks, 490);
		check((breaks.count == 6), "Breakpoints count not increasing properly");
		add_breakpoint(&breaks, 795);
		check((breaks.count == 6), "Breakpoints inserting duplicate points");
	}

	// TEST split_str()
	{
		char** split;
		split = split_str("hello world");
		check((split[0][0] == 2), "split_str() wrong word count for 'hello world'");
		check((split[0][1] == 0), "split_str() wrong start of 'hello'");
		check((split[0][2] == 6), "split_str() wrong start of 'world'");
		check((0 == strcmp(split[1], "hello")), "split_str() word for 'hello'");
		check((0 == strcmp(split[2], "world")), "split_str() word for 'world'");

		split = split_str("    3258967  ");
		check((split[0][0] == 1), "split_str() wrong word count for '    3258967  '");
		check((split[0][1] == 4), "split_str() wrong start of '3258967'");
		check((0 == strcmp(split[1], "3258967")), "split_str() word for '3258967'");

		split = split_str("set a0a  f0&; ");
		check((split[0][0] == 3), "split_str() wrong word count for 'set a0a  f0&; '");
		check((split[0][1] == 0), "split_str() wrong start of 'set'");
		check((split[0][2] == 4), "split_str() wrong start of 'a0a'");
		check((split[0][3] == 9), "split_str() wrong start of 'f0&;'");
		check((0 == strcmp(split[1], "set")), "split_str() word for 'set'");
		check((0 == strcmp(split[2], "a0a")), "split_str() word for 'a0a'");
		check((0 == strcmp(split[3], "f0&;")), "split_str() word for 'f0&;'");

		split = split_str(" a b c d e f g");
		check((split[0][0] == 7), "split_str() wrong word count for ' a b c d e f g'");
		check((split[0][1] == 1), "split_str() wrong start of 'a'");
		check((split[0][2] == 3), "split_str() wrong start of 'b'");
		check((split[0][3] == 5), "split_str() wrong start of 'c'");
		check((split[0][4] == 7), "split_str() wrong start of 'd'");
		check((split[0][5] == 9), "split_str() wrong start of 'e'");
		check((split[0][6] == 11), "split_str() wrong start of 'f'");
		check((split[0][7] == 13), "split_str() wrong start of 'g'");
		check((0 == strcmp(split[1], "a")), "split_str() word for 'a'");
		check((0 == strcmp(split[2], "b")), "split_str() word for 'b'");
		check((0 == strcmp(split[3], "c")), "split_str() word for 'c'");
		check((0 == strcmp(split[4], "d")), "split_str() word for 'd'");
		check((0 == strcmp(split[5], "e")), "split_str() word for 'e'");
		check((0 == strcmp(split[6], "f")), "split_str() word for 'f'");
		check((0 == strcmp(split[7], "g")), "split_str() word for 'g'");

		split = split_str("first \n second");
		check((split[0][0] == 1), "split_str() wrong word count for 'first \\n second'");
		check((split[0][1] == 0), "split_str() wrong start of 'first'");
		check((0 == strcmp(split[1], "first")), "split_str() word for 'first'");
	}

	// TEST debug()
	{
		ibcmemory data;
		for (int i = 0; i < MEM_SIZE; i++)
			data.mem[i] = 0xb000;
		data.acc = 0;
		data.pc = 0;
		breakpoints breaks = init_breakpoints();

		unsigned char result;
		// Pause STDOUT
		fpos_t pos;
		fgetpos(stdout, &pos);
		fclose(stdout);

		// Step
		result = debug(&data, &breaks, "\n", 0);
		check((result == 1), "debug() returns wrong code after step (blank)");
		check((data.pc == 1), "debug() does not preform step (blank)");

		result = debug(&data, &breaks, "step \n", 0);
		check((result == 1), "debug() returns wrong code after step (1)");
		check((data.pc == 2), "debug() does not preform step (1)");

		result = debug(&data, &breaks, "step 10 \n", 0);
		check((result == 1), "debug() returns wrong code after step 12");
		check((data.pc == 12), "debug() does not preform step 12");

		// Run
		result = debug(&data, &breaks, "run \n", 0);
		check((result == 0), "debug() returns wrong code after step 12");
		check((data.pc == 13), "debug() does not preform step 12");

		// Restart STDOUT
		stdout = fdopen(0, "w");
		fsetpos(stdout, &pos);
	}

	if (FAILED > 0)
		printf("\n-------------------------------------------------------------\n");

	printf("Tests Passed: %d\n", PASSED);
	if (FAILED > 0) {
		printf("Tests Failed: \33[31m%d\33[0m\n", FAILED);
		return 1;
	}

	return 0;
}
