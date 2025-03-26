const std = @import("std");
const executor = @import("executor.zig");

pub fn main() anyerror!void {
    // Get allocator
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();
    // Process cli arguements
    const stdout = std.io.getStdOut().writer();
    const args = try std.process.argsAlloc(allocator);
    defer std.process.argsFree(allocator, args);
    if (args.len <= 1) {
        try stdout.print("Missing code file\n", .{});
        std.os.linux.exit(1);
    }

    var ibcm = try executor.IBCM.from_file(&args[1]);
    try ibcm.run();
}
