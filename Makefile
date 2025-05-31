# IBCMer - Central Makefile
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>

all: c/ibcmer.out cpp/ibcmer.out go/ibcmer rust/target/release/ibcmer zig/zig-out/bin/ibcmer javascript/node_modules swift/.build/release/ibcmer

test: all
	make -C c test
	cd rust && cargo test --release
	cd zig && zig test src/main.zig
	./tests/run.sh

clean:
	make -C c clean
	make -C cpp clean
	rm -f go/ibcmer
	rm -rf rust/target
	rm -rf zig/.zig-cache zig/zig-out
	rm -rf js/node_modules
	rm -rf swift/.build


c/ibcmer.out: c/ibcmer.c c/src
	make -C c ibcmer.out

cpp/ibcmer.out: cpp/ibcmer.cpp cpp/src
	make -C cpp ibcmer.out

go/ibcmer: go/go.mod go/*.go
	cd go && go build

rust/target/release/ibcmer: rust/Cargo.toml rust/src
	cd rust && cargo build --release

zig/zig-out/bin/ibcmer: zig/src/* zig/build.zig*
	cd zig && zig build --release=fast

javascript/node_modules: javascript/package-lock.json
	cd javascript && npm clean-install

swift/.build/release/ibcmer: swift/Package.swift swift/Sources/*
	cd swift && swift build -c release
