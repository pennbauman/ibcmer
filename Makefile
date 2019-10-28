# Penn Bauman
# pennbauman@protonmail.com

CXX=clang++

ibcmer: ibcmer.cpp out/program.o out/doubleByte.o out/sortStack.o out/intNode.o
	$(CXX) ibcmer.cpp out/*.o

final:
	$(CXX) -O2 ibcmer.cpp program.cpp doubleByte.cpp sortStack.cpp intNode.cpp -o ibcmer

clean:
	-rm -f out/*.o a.out ibcmer

out/program.o: program.cpp sortStack.cpp \
		program.h sortStack.h
	@mkdir -p out
	$(CXX) -c program.cpp -o out/program.o

out/doubleByte.o: doubleByte.cpp doubleByte.h
	@mkdir -p out
	$(CXX) -c doubleByte.cpp -o out/doubleByte.o

out/sortStack.o: sortStack.cpp intNode.cpp \
		sortStack.h intNode.h
	@mkdir -p out
	$(CXX) -c sortStack.cpp -o out/sortStack.o

out/intNode.o: intNode.cpp intNode.h
	@mkdir -p out
	$(CXX) -c intNode.cpp -o out/intNode.o
