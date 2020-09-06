# Makefile - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author:
#     Penn Bauman (pennbauman@protonmail.com)
CC=clang

ibcmer: ibcmer.c text.h
	$(CC) ibcmer.c -o ibcmer

test: ibcmer
	./ibcmer test.ibcm -s
