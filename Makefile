# Makefile - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author:
#     Penn Bauman (pennbauman@protonmail.com)
CC=clang
COMP_DIR=compile

ibcmer: ibcmer.c text.h $(COMP_DIR)/executor.o $(COMP_DIR)/debugger.o
	$(CC) ibcmer.c $(COMP_DIR)/executor.o $(COMP_DIR)/debugger.o -o ibcmer

$(COMP_DIR)/executor.o: executor.c executor.h
	@mkdir -p $(COMP_DIR)
	$(CC) -c executor.c -o $(COMP_DIR)/executor.o

$(COMP_DIR)/debugger.o: debugger.c debugger.h
	@mkdir -p $(COMP_DIR)
	$(CC) -c debugger.c -o $(COMP_DIR)/debugger.o

test: ibcmer
	./ibcmer test.ibcm -s

clean:
	@rm -rf $(COMP_DIR)
	@rm -f ibcmer
