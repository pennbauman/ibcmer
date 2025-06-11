// IBCMer - Zig
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
const std = @import("std");

const MEM_SIZE = 4096;


pub const IBCMError = error {
    FileNotFound,
    InvalidHex,
    PCOverflow,
    InvalidOpcode,
    ParseOverflow,
    Internal,
};

// Result from parsing hex, u16 result or error index
const HexResultTag = enum {
    parsed,
    err,
};
const HexResult = union(enum) {
    parsed: u16,
    err: usize,
};

pub const IBCM = struct {
    pc: u16,
    accumulator: u16,
    memory: [MEM_SIZE]u16,
    halted: bool,

    pub fn init() IBCM {
        return IBCM {
            .pc = 0,
            .accumulator = 0,
            .memory = [_]u16{0} ** MEM_SIZE,
            .halted = false,
        };
    }

    pub fn from_file(filepath: *const [:0]u8) anyerror!IBCM {
        var gpa = std.heap.GeneralPurposeAllocator(.{}){};
        defer _ = gpa.deinit();
        const allocator = gpa.allocator();
        const stderr = std.io.getStdErr().writer();
        // open file
        var file = std.fs.cwd().openFile(filepath.*, .{}) catch {
            try stderr.print("Error: Code file '{s}' not found\n", .{filepath.*});
            return IBCMError.FileNotFound;
        };
        defer file.close();
        var buf_reader = std.io.bufferedReader(file.reader());
        const reader = buf_reader.reader();
        var line = std.ArrayList(u8).init(allocator);
        defer line.deinit();
        const writer = line.writer();

        var ret = IBCM.init();
        var i: usize = 0;
        while (reader.streamUntilDelimiter(writer, '\n', null)) {
            if (i >= MEM_SIZE) {
                try stderr.print("Error: Code file overflows memory ({d} lines max)\n", .{MEM_SIZE});
                return IBCMError.ParseOverflow;
            }
            defer line.clearRetainingCapacity();
            if (line_to_mem(&ret, line.items, i)) |index| {
                try parse_err_print(filepath, i, index, line.items);
                return IBCMError.InvalidOpcode;
            }
            i += 1;
        } else |err| switch (err) {
            error.EndOfStream => { // end of file
                if (line.items.len > 0) {
                    if (line_to_mem(&ret, line.items, i)) |index| {
                        try parse_err_print(filepath, i, index, line.items);
                        return IBCMError.InvalidOpcode;
                    }
                    i += 1;
                }
            },
            else => return err,
        }
        return ret;
    }
    // Populates memory from hex string, return error index on failure
    fn line_to_mem(self: *IBCM, line: []u8, number: usize) ?usize {
        if (line.len < 4) {
            return line.len;
        }
        const int = switch (hex_to_int(line)) {
            .parsed => |n| n,
            .err => |i| return i,
        };
        self.memory[number] = int;
        return null;
    }
    fn parse_err_print(file: *const [:0]u8, line: usize, index: usize, text: []u8) anyerror!void {
        const stderr = std.io.getStdErr().writer();
        try stderr.print("Error: '{s}:{d}:{d}' Invalid opcode hexadecimal\n\n    ", .{file.*, line + 1, index + 1});
        for (text) |c| {
            try stderr.print("{c}", .{c});
        }
        try stderr.print("\n    ", .{});
        var j: usize = 0;
        while (j < index) {
            try stderr.print(" ", .{});
            j += 1;
        }
        try stderr.print("^\n", .{});
    }

    pub fn print(self: IBCM) anyerror!void {
        const stdout = std.io.getStdOut().writer();
        try stdout.print("ACC = {d:0>4}\n", .{self.accumulator});
        try stdout.print("PC  =  {d:0>3}\n", .{self.pc});
        try stdout.print("----------\n", .{});
        var max: usize = MEM_SIZE - 1;
        while (self.memory[max] == 0 and max > 0) {
            max -= 1;
        }
        for (self.memory, 0..MEM_SIZE) |data, i| {
            try stdout.print("[{x:0>3}] {x:0>4}\n", .{i, data});
            if (i > max) break;
        }
    }

    pub fn run(self: *IBCM) anyerror!void {
        while (try self.step()) {}
    }

    fn step(self: *IBCM) anyerror!bool {
        const stdout = std.io.getStdOut().writer();
        const stderr = std.io.getStdErr().writer();
        const opcode = self.memory[self.pc] >> 12;
        const address = self.memory[self.pc] & 0xfff;

        try stdout.print("[{x:0>3}]{x:0>4}  ", .{self.pc, self.memory[self.pc]});
        switch (opcode) {
            0x0 => { // halt
                try stdout.print("halt\n", .{});
                self.halted = true;
                return false;
            },
            0x1 => { // i/o
                try stdout.print("i/o   (ACC){x:0>4}\n", .{self.accumulator});
                const subopcode = address >> 8;
                switch (subopcode) {
                    0x0 => {
                        while (true) {
                            try stdout.print("Input hex:  ", .{});
                            const input = get_stdin() catch |err| {
                                return err;
                            };
                            if (input[0] < 14) {
                                try stdout.print("Missing hex input\n", .{});
                                continue;
                            }
                            var len: usize = 1;
                            while (input[len] != '\n') {
                                if (input[len] < 14) {
                                    try stdout.print("'0'\n", .{});
                                    return IBCMError.Internal;
                                }
                                if (len > 4) {
                                    try stdout.print("Invalid hex input (too long) {s}\n", .{input});
                                    return IBCMError.Internal;
                                    // break;
                                }
                                len += 1;
                            }
                            if (len > 4) {
                                continue;
                            }
                            var hex: [4]u8 = undefined;
                            var i: usize = 0;
                            while (i < len) {
                                hex[4 - len + i] = input[i];
                                i += 1;
                            }
                            i = 0;
                            while (len + i < 4) {
                                hex[i] = '0';
                                i += 1;
                            }
                            const val = switch (hex_to_int(&hex)) {
                                .parsed => |n| n,
                                .err => {
                                    try stdout.print("Invalid hex input\n", .{});
                                    continue;
                                },
                            };
                            // std.debug.print("{s}", .{hex});
                            self.accumulator = val;
                            break;
                        }
                    },
                    0x4 => {
                        while (true) {
                            try stdout.print("Input char: ", .{});
                            const input = get_stdin() catch |err| {
                                return err;
                            };
                            if (input[1] == '\n') {
                                self.accumulator = input[0];
                                break;
                            } else {
                                try stdout.print("Invalid char input\n", .{});
                            }
                        }
                    },
                    0x8 => {
                        try stdout.print("Output hex:  {x:0>4}\n", .{self.accumulator});
                    },
                    0xC => {
                        const char: u8 = @truncate(self.accumulator);
                        try stdout.print("Output char: {c}\n", .{char});
                    },
                    else => {
                        try stdout.print("\n", .{});
                        try stderr.print("Error: Invalid I/O sub-opcode '{x}'\n", .{subopcode});
                        return IBCMError.InvalidOpcode;
                    },
                }
            },
            0x2 => { // shift
                const subopcode = address >> 8;
                const distance: u4 = @truncate(address & 0xf);
                const prev = self.accumulator;
                var arrow: *const [2:0]u8 = undefined;
                switch (subopcode) {
                    0x0 => {
                        self.accumulator = self.accumulator << distance;
                        arrow = "<<";
                    },
                    0x4 => {
                        self.accumulator = self.accumulator >> distance;
                        arrow = ">>";
                    },
                    0x8 => {
                        self.accumulator = (self.accumulator << distance) | (self.accumulator >> (0 -% distance));
                        arrow = "<=";
                    },
                    0xC => {
                        self.accumulator = (self.accumulator >> distance) | (self.accumulator << (0 -% distance));
                        arrow = "=>";
                    },
                    else => {
                        try stdout.print("shift \n", .{});
                        try stderr.print("Error: Invalid shift sub-opcode '{x}'\n", .{subopcode});
                        return IBCMError.InvalidOpcode;
                    },
                }
                try stdout.print("shift (ACC){x:0>4} = (ACC){x:0>4} {s} {x}\n", .{self.accumulator, prev, arrow, distance});
            },
            0x3 => { // load
                self.accumulator = self.memory[address];
                try stdout.print("load  (ACC){x:0>4} = [{x:0>3}]{x:0>4}\n", .{self.accumulator, address, self.accumulator});
            },
            0x4 => { // store
                self.memory[address] = self.accumulator;
                try stdout.print("store [{x:0>3}]{x:0>4} = (ACC){x:0>4}\n", .{address, self.accumulator, self.accumulator});
            },
            0x5 => { // add
                const prev = self.accumulator;
                self.accumulator = self.accumulator +% self.memory[address];
                try stdout.print("add   (ACC){x:0>4} = (ACC){x:0>4} + [{x:0>3}]{x:0>4}\n", .{self.accumulator, prev, address, self.memory[address]});
            },
            0x6 => { // sub
                const prev = self.accumulator;
                self.accumulator = self.accumulator -% self.memory[address];
                try stdout.print("sub   (ACC){x:0>4} = (ACC){x:0>4} - [{x:0>3}]{x:0>4}\n", .{self.accumulator, prev, address, self.memory[address]});
            },
            0x7 => { // and
                const prev = self.accumulator;
                self.accumulator = self.accumulator & self.memory[address];
                try stdout.print("and   (ACC){x:0>4} = (ACC){x:0>4} & [{x:0>3}]{x:0>4}\n", .{self.accumulator, prev, address, self.memory[address]});
            },
            0x8 => { // or
                const prev = self.accumulator;
                self.accumulator = self.accumulator | self.memory[address];
                try stdout.print("or    (ACC){x:0>4} = (ACC){x:0>4} | [{x:0>3}]{x:0>4}\n", .{self.accumulator, prev, address, self.memory[address]});
            },
            0x9 => { // xor
                const prev = self.accumulator;
                self.accumulator = self.accumulator ^ self.memory[address];
                try stdout.print("xor   (ACC){x:0>4} = (ACC){x:0>4} ^ [{x:0>3}]{x:0>4}\n", .{self.accumulator, prev, address, self.memory[address]});
            },
            0xA => { // not
                const prev = self.accumulator;
                self.accumulator = self.accumulator ^ 0xffff;
                try stdout.print("not   (ACC){x:0>4} = ! (ACC){x:0>4}\n", .{self.accumulator, prev});
            },
            0xB => { // nop
                try stdout.print("nop\n", .{});
            },
            0xC => { // jmp
                try stdout.print("jmp   [{x:0>3}]\n", .{address});
                self.pc = address - 1;
            },
            0xD => { // jmpe
                if (self.accumulator == 0) {
                    try stdout.print("jmpe  [{x:0>3}]\n", .{address});
                    self.pc = address - 1;
                } else {
                    try stdout.print("jmpe  (ACC){x:0>4}\n", .{self.accumulator});
                }
            },
            0xE => { // jmpl
                if (self.accumulator >> 15 == 1) {
                    try stdout.print("jmpl  [{x:0>3}]\n", .{address});
                    self.pc = address - 1;
                } else {
                    try stdout.print("jmpl  (ACC){x:0>4}\n", .{self.accumulator});
                }
            },
            0xF => { // brl
                try stdout.print("brl   [{x:0>3}]  (ACC){x:0>4}\n", .{address, self.pc + 1});
                self.accumulator = self.pc + 1;
                self.pc = address - 1;
            },
            else => {
                try stderr.print("Error: Invalid opcode\n", .{});
                return IBCMError.Internal;
            }
        }
        self.pc += 1;
        if (self.pc >= MEM_SIZE) {
            try stderr.print("Error: Memory overflow (PC = 0x{x:0>4})\n", .{MEM_SIZE});
            return IBCMError.PCOverflow;
        }
        return true;
    }
};


fn get_stdin() anyerror![4096]u8 {
    const in = std.io.getStdIn();
    var reader = in.reader();

    var buf: [4096]u8 = undefined;
    _ = try reader.readUntilDelimiterOrEof(&buf, '\n');
    return buf;
}

// Parses hex string to u16, if it fails returns the index of the error
fn hex_to_int(line: []u8) HexResult {
    if (line.len < 4) {
        return HexResult{ .err = line.len };
    }
    var hex: [4]u16 = undefined;
    for (0..4) |i| {
        hex[i] = switch (line[i]) {
            '0'...'9' => line[i] - '0',
            'A'...'F' => line[i] - 'A' + 10,
            'a'...'f' => line[i] - 'a' + 10,
            else => return HexResult{ .err = i },
        };
    }
    const res = hex[3] + (hex[2] << 4) + (hex[1] << 8) + (hex[0] << 12);
    return HexResult{ .parsed = res };
}

// Tests
test "hex_to_int 'FFFF'" {
    var line = std.ArrayList(u8).init(std.testing.allocator);
    defer line.deinit();
    try line.appendSlice("FFFF");
    const res = try hex_to_int(line.items);
    try std.testing.expect(res == 0xffff);
}
test "hex_to_int 'Aa09  '" {
    var line = std.ArrayList(u8).init(std.testing.allocator);
    defer line.deinit();
    try line.appendSlice("Aa09  ");
    const res = try hex_to_int(line.items);
    try std.testing.expect(res == 0xaa09);
}
test "hex_to_int 'Aa0' too short" {
    var line = std.ArrayList(u8).init(std.testing.allocator);
    defer line.deinit();
    try line.appendSlice("Aa0");
    _ = hex_to_int(line.items) catch |err| {
        try std.testing.expect(err == IBCMError.InvalidHex);
        return;
    };
    try std.testing.expect(false);
}
test "hex_to_int '    ' blank" {
    var line = std.ArrayList(u8).init(std.testing.allocator);
    defer line.deinit();
    try line.appendSlice("    ");
    _ = hex_to_int(line.items) catch |err| {
        try std.testing.expect(err == IBCMError.InvalidHex);
        return;
    };
    try std.testing.expect(false);
}
