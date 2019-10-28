# Penn Bauman
# pennbauman@protonmail.com

CXX=clang++

ibcmer: ibcmer.cpp out/program.o out/doubleByte.o
	$(CXX) ibcmer.cpp out/*.o

final:
	$(CXX) -O2 ibcmer.cpp program.cpp doubleByte.cpp -o ibcmer

clean:
	-rm -f out/*.o a.out ibcmer

out/program.o: program.cpp doubleByte.cpp \
		program.h doubleByte.h
	@mkdir -p out
	$(CXX) -c program.cpp -o out/program.o

out/doubleByte.o: doubleByte.cpp doubleByte.h
	@mkdir -p out
	$(CXX) -c doubleByte.cpp -o out/doubleByte.o
