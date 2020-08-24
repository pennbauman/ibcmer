# Makefile - IBCMer
#   Penn Bauman
#   pennbauman@protonmail.com
CC=clang

ibcmer: ibcmer.c text.h
	$(CC) ibcmer.c -o ibcmer

test: ibcmer
	./ibcmer test.ibcm -s
