# IBCMer - Central Makefile
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>

all: c/ibcmer.out cpp/ibcmer.out go/ibcmer rust/target/release/ibcmer zig/zig-out/bin/ibcmer

test: all
	./tests/run.sh

clean:
	make -C c clean
	make -C cpp clean
	cd go && go clean
	cd rust && cargo clean
	rm -rf zig/.zig-cache zig/zig-out


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
