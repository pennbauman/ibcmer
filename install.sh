#!/bin/sh
# Install - IBCMer
#   Penn Bauman
#   pennbauman@protonmail.com

echo -n "Compiling..."
gcc -O2 ibcmer.c -o ibcmer
echo "Done"

echo -n "Testing..."
output=$(./ibcmer tests/all-ops.ibcm -c -q <<< a)
if [[ "${output:23:4}" = "19ed" ]] && [[ "${output:28:1}" == "!" ]]; then
	echo "Passed"
else
	echo -e "\033[0;31mError\033[0m"
	echo "  Expected: '19ed !'"
	echo "  Received: '${output:23:4} ${output:28:1}'"
	exit 1
fi

echo Installing
#sudo cp ibcmer /bin/
