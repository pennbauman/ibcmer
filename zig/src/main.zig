// IBCMer - Zig
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
const std = @import("std");
const executor = @import("executor.zig");

pub fn main() anyerror!void {
    // Get allocator
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();
    // Process cli arguements
    const stderr = std.io.getStdErr().writer();
    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);
    if (args.len <= 1) {
        try stderr.print("Missing code file\n", .{});
        std.os.linux.exit(1);
    }

    var ibcm = executor.IBCM.from_file(&args[1]) catch |err| {
        try stderr.print("Error: {:0}\n", .{err});
        std.os.linux.exit(1);
    };
    ibcm.run() catch |err| {
        try stderr.print("Error: {:0}\n", .{err});
        std.os.linux.exit(1);
    };
}
