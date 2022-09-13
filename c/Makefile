# Makefile - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author:
#     Penn Bauman (pennbauman@protonmail.com)
CC=clang
BLD_DIR=build
SRC_DIR=src

ibcmer: ibcmer.c $(SRC_DIR)/text.h $(BLD_DIR)/executor.o $(BLD_DIR)/debugger.o
	$(CC) ibcmer.c $(BLD_DIR)/executor.o $(BLD_DIR)/debugger.o -o ibcmer

$(BLD_DIR)/executor.o: $(SRC_DIR)/executor.c $(SRC_DIR)/executor.h
	@mkdir -p $(BLD_DIR)
	$(CC) -c $(SRC_DIR)/executor.c -o $(BLD_DIR)/executor.o

$(BLD_DIR)/debugger.o: $(SRC_DIR)/debugger.c $(SRC_DIR)/debugger.h
	@mkdir -p $(BLD_DIR)
	$(CC) -c $(SRC_DIR)/debugger.c -o $(BLD_DIR)/debugger.o

run: ibcmer
	./ibcmer test/all-ops.ibcm -s

test: ibcmer test/units FORCE
	@echo ""
	@./test/units
	@echo ""
	@./test/system.sh

test/units: $(SRC_DIR)/unit_tests.c $(BLD_DIR)/executor.o $(BLD_DIR)/debugger.o
	$(CC) $(SRC_DIR)/unit_tests.c $(BLD_DIR)/executor.o $(BLD_DIR)/debugger.o -o test/units

clean:
	@rm -rf $(BLD_DIR)
	@rm -f ibcmer
	@rm -f test/units

FORCE:
