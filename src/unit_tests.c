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


	if (FAILED > 0)
		printf("\n-------------------------------------------------------------\n");

	printf("Tests Passed: %d\n", PASSED);
	if (FAILED > 0) {
		printf("Tests Failed: \33[31m%d\33[0m\n", FAILED);
		return 1;
	}

	return 0;
}
