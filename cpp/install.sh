#!/bin/sh
# Penn Bauman
# pennbauman@protonmail.com

if clang++ --version &> /dev/null; then
	echo "Making (clang++)"
	clang++ -O2 ibcmer.cpp program.cpp doubleByte.cpp -o ibcmer
else
	echo "Making (g++)"
	g++ -O2 ibcmer.cpp program.cpp doubleByte.cpp -o ibcmer
fi

sudo cp ibcmer /bin/
