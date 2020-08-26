#!/bin/sh
# Install - IBCMer
#   Penn Bauman
#   pennbauman@protonmail.com

echo -n "Compiling..."
gcc -O2 ibcmer.c -o ibcmer
echo "Done"

echo -n "Testing..."
output=$(./ibcmer test.ibcm --check --silent <<< a)
if [[ "$output" = "19ed!" ]]; then
	echo "Passed"
else
	echo -e "\033[0;31mError\033[0m"
	echo "  Expected: '19ed!'"
	echo "  Received: '$output'"
	exit 1
fi

echo Installing
sudo cp ibcmer /bin/
