#!/bin/sh
# Install - IBCMer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author:
#     Penn Bauman (pennbauman@protonmail.com)

echo -n "Compiling..."
temp_file=".temp"
script -c "gcc -Wno-unused-result -O2 ibcmer.c src/executor.c src/debugger.c -o ibcmer" -e -q --quiet $temp_file &> /dev/null
if [ $? -eq 0 ]; then
	echo "Done"
	rm $temp_file
else
	echo -e "\033[0;31mError\033[0m"
	echo ""
	len=$(cat $temp_file | wc -l)
	echo -e "$(cat $temp_file | tail -n $(($len-1)) | head -n $(($len-3)))"
	rm $temp_file
	exit 1
fi


echo -n "Testing..."
test_output=$(./ibcmer test.ibcm --check --silent <<< a)
if [[ "$test_output" = "19ed!" ]]; then
	echo "Passed"
else
	echo -e "\033[0;31mError\033[0m"
	echo "  Expected: '19ed!'"
	echo "  Received: '$test_output'"
	exit 1
fi


echo Installing
sudo cp ibcmer /bin/
