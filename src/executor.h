// IBCMer - IBCM Executor
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "text.h" // Defines text variables

// Size of IBCM memory
#define MEM_SIZE 4096

// IBCM memory and registers
typedef struct ibcmemory {
	unsigned short acc; // = 0; // Accumulator
	unsigned short pc; // = 0; // Program counter
	unsigned short mem[MEM_SIZE]; // Program memory
} ibcmemory;

// Initialize memory
ibcmemory init_ibcmemory();
// Execute one command and progess the program
void step(ibcmemory *data, int volume);

#endif
