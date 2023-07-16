# IBCMer - Central Makefile
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>

all: c/ibcmer cpp/ibcmer go/ibcmer rust/target/release/ibcmer

test: all
	./tests/run.sh

clean:
	make -C c clean
	make -C cpp clean
	rm -f go/ibcmer
	rm -rf rust/target


c/ibcmer: c/ibcmer.c c/src
	make -C c

cpp/ibcmer: cpp/ibcmer.cpp cpp/src
	make -C cpp

go/ibcmer: go/go.mod go/*.go
	go build -C go

rust/target/release/ibcmer: rust/Cargo.toml rust/src
	cd rust && cargo build --release
