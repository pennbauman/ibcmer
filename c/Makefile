# Makefile - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
CC = clang
CFLAGS = -g -Wall -std=c17 -O3 -fPIC
prefix = /usr/local


ibcmer.out: ibcmer.c src/executor.o src/debugger.o src/text.h
	$(CC) $(CFLAGS) -pie $< src/*.o -o $@

src/executor.o: src/executor.c src/executor.h

src/debugger.o: src/debugger.c src/debugger.h

test/units.out: test/units.c src/executor.o src/debugger.o
	$(CC) $(CFLAGS) -pie $< src/*.o -o $@


.PHONY: clean test install
clean:
	@rm -f *.out */*.out */*.o

test: ibcmer.out test/units.out
	@echo ""
	@./test/units.out
	@echo ""
	@./test/system.sh

install: ibcmer.out
	install -m 0755 -d $(DESTDIR)$(prefix)/bin
	install -m 0755 ibcmer.out $(DESTDIR)$(prefix)/bin/ibcmer

remove:
	rm -f $(DESTDIR)$(prefix)/bin/ibcmer
