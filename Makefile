# Penn Bauman
# pennbauman@protonmail.com
CC=clang

ibcmer: ibcmer.c
	$(CC) ibcmer.c -o ibcmer

test: ibcmer
	./ibcmer tests/summation.ibcm
