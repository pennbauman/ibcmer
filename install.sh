#!/bin/sh
# Install - IBCMer
#   Penn Bauman
#   pennbauman@protonmail.com

echo Compiling
gcc -O2 ibcmer.c -o ibcmer
echo Installing
sudo cp ibcmer /bin/
