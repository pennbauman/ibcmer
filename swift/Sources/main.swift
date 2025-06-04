// IBCMer - Swift
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
import Foundation

struct StandardError: TextOutputStream, Sendable {
    private static let handle = FileHandle.standardError

    public func write(_ string: String) {
        Self.handle.write(Data(string.utf8))
    }
}

var stderr = StandardError()

if (Swift.CommandLine.arguments.count < 2) {
    Swift.print("Missing code file")
} else {
    do {
        var machine = try IBCM.fromFile(path: Swift.CommandLine.arguments[1])
        try machine.run()
    } catch {
        Swift.print("Error:", error, to: &stderr)
    }
}



