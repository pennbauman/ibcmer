// IBCMer - Swift
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>

if (Swift.CommandLine.arguments.count < 2) {
    Swift.print("Missing code file")
} else {
    do {
        var machine = try IBCM.fromFile(path: Swift.CommandLine.arguments[1])
        try machine.run()
    } catch {
        Swift.print("ERROR", error)
    }
}



