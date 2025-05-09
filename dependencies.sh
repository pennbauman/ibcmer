#!/bin/sh
# IBCMer - Dependency Installer
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>

if [ $(command -v dnf) ]; then
	sudo dnf install -y --setopt=install_weak_deps=False gcc g++ go lua perl python3 cargo R-core R-bitops rubypick rubygems zig nodejs
elif [ $(command -v apt) ]; then
	sudo apt update
	sudo apt install -y --no-install-recommends make gcc g++ golang lua5.4 perl python3 cargo r-base-core r-cran-bitops ruby nodejs npm
	if [ ! $(command -v zig) ]; then
		echo "zig still required"
	fi
else
	echo "Unknown packager"
	exit 1
fi
