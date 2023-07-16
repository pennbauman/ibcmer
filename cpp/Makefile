# Makefile - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
CXXFLAGS = -g -Wall -std=c++17 -O3 -fPIC
prefix = /usr/local

ibcmer.out: ibcmer.cpp src/program.o src/doubleByte.o
	$(CXX) $(CXXFLAGS) -pie $< src/*.o -o $@

src/doubleByte.o: src/doubleByte.cpp src/doubleByte.h

src/program.o: src/program.cpp src/program.h

.PHONY: clean install
clean:
	@rm -f *.out */*.o

install: ibcmer.out
	install -m 0755 -d $(DESTDIR)$(prefix)/bin
	install -m 0755 ibcmer.out $(DESTDIR)$(prefix)/bin/ibcmer

remove:
	rm -f $(DESTDIR)$(prefix)/bin/ibcmer
