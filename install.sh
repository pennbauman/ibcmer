#!/bin/sh
# Install - IBCMer
#   Penn Bauman
#   pennbauman@protonmail.com

echo -n "Compiling..."
temp=".temp"
script -c "gcc -Wno-unused-result -O2 ibcmer.c -o ibcmer" -e -q --quiet $temp &> /dev/null
if [ $? -eq 0 ]; then
	echo "Done"
	rm $temp
else
	echo -e "\033[0;31mError\033[0m"
	echo ""
	len=$(cat $temp | wc -l)
	echo -e "$(cat $temp | tail -n $(($len-1)) | head -n $(($len-3)))"
	rm $temp
	exit 1
fi

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
