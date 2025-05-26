// IBCMer - Swift Library
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
import Foundation

let MEM_SIZE = 4096
let PRINT_COLUMNS = 8

enum IBCMError: Error, Equatable {
    case fileError
    case memoryOverflow
    case parseError(line: Int, text: String)
    case invalidHex
    case invalidOpcode
}


func hexToInt(hex: String) throws -> UInt16 {
    let num = UInt16(strtoul("0x" + hex, nil, 16))
    let reverse = String(format:"%0" + String(hex.count) + "x", num)
    if (reverse != hex.lowercased()) {
        throw IBCMError.invalidHex
    }
    return num
}


struct IBCM {
    var pc: UInt16 = 0
    var accumulator: UInt16 = 0
    var memory: [UInt16] = Array()
    var halted = false

    init() {
        memory = Array(repeating: 0, count: 4096)
    }

    static func fromFile(path: String) throws -> IBCM {
        var ret = IBCM()
        if let handle = FileHandle(forReadingAtPath: path) {
            defer {
                handle.closeFile()
            }
            let data = try handle.readToEnd() ?? Data()
            if let content = String(data: data, encoding: .utf8) {
                let lines = content.split(separator: "\n", omittingEmptySubsequences: false)
                var i = 0
                for line in lines {
                    // Check for short lines
                    if (line.count == 0) {
                        continue
                    }
                    if (i == MEM_SIZE) {
                        throw IBCMError.memoryOverflow
                    }
                    if (line.count < 4) {
                        throw IBCMError.parseError(line: i, text: String(line))
                    }
                    // Parse opcode
                    let index = line.index(line.startIndex, offsetBy: 3)
                    let opcode = String(line.prefix(through: index))
                    do {
                        let num = try hexToInt(hex: opcode)
                        ret.memory[i] = num
                    } catch {
                        throw IBCMError.parseError(line: i, text: String(opcode))
                    }
                    i += 1
                }
            } else {
                throw IBCMError.fileError
            }
        } else {
            throw IBCMError.fileError
        }
        return ret
    }

    func print() {
        Swift.print(String(format: "IBCMer     ACC: %04x  PC:  %03x", self.accumulator, self.pc))
        // Determine last used memory
        var last = MEM_SIZE - 1
        while self.memory[last] == 0 {
            if last == 0 {
                break
            }
            last -= 1
        }
        // Print memory
        var column = 0
        for addr in 0...(MEM_SIZE - 1) {
            Swift.print(String(format: "[%03x]%04x  ", addr, self.memory[addr]), terminator: "")
            column += 1
            if column == PRINT_COLUMNS {
                Swift.print("")
                column = 0
                if addr > last {
                    break
                }
            }
        }
    }

    mutating func step() throws {
        let op = self.memory[Int(self.pc)]
        let addr = op & 0x0FFF
        Swift.print(String(format: "[%03x]%04x  ", self.pc, op), terminator: "")
        switch op >> 12 {
            case 0x0: // halt
                Swift.print("halt")
                self.halted = true
                return
            case 0x1: // i/o
                Swift.print(String(format: "i/o   (ACC)%04x", self.accumulator))
                switch addr >> 8 {
                    case 0x0:
                        while true {
                            Swift.print("Input hex:  ", terminator: "")
                            if let text = Swift.readLine() {
                                if text.count > 4 {
                                    Swift.print("  hexidecimal too long")
                                    continue
                                }
                                do {
                                    let num = try hexToInt(hex: text)
                                    self.accumulator = num
                                    break
                                } catch {
                                    Swift.print("  invalid hexidecimal")
                                    continue
                                }
                            }
                        }
                    case 0x4:
                        while true {
                            Swift.print("Input char: ", terminator: "")
                            if let text = Swift.readLine() {
                                if text.count == 1 {
                                    if let c = text.first {
                                        if let ascii = c.asciiValue {
                                            let num = UInt16(ascii)
                                            self.accumulator = num
                                            break
                                        }
                                    }
                                } else {
                                    Swift.print("  input only one character")
                                }
                            } else {
                                Swift.print("  character required")
                            }
                        }
                    case 0x8:
                        Swift.print(String(format: "Output hex:  %04x", self.accumulator))
                    case 0xC:
                        let c = Character(UnicodeScalar(self.accumulator) ?? "#")
                        Swift.print("Output char: " + String(c))
                    default:
                        throw IBCMError.invalidOpcode
                }
            case 0x2: // shift
                let old = self.accumulator
                let distance = op & 0x000F
                switch addr >> 8 {
                    case 0x0:
                        self.accumulator = self.accumulator << distance
                        Swift.print(String(format: "shift (ACC)%04x = (ACC)%04x << %x", self.accumulator, old, distance))
                    case 0x4:
                        self.accumulator = self.accumulator >> distance
                        Swift.print(String(format: "shift (ACC)%04x = (ACC)%04x >> %x", self.accumulator, old, distance))
                    case 0x8:
                        self.accumulator = (self.accumulator << distance) | (self.accumulator >> (16 - distance))
                        Swift.print(String(format: "shift (ACC)%04x = (ACC)%04x <= %x", self.accumulator, old, distance))
                    case 0xC:
                        self.accumulator = (self.accumulator >> distance) | (self.accumulator << (16 - distance))
                        Swift.print(String(format: "shift (ACC)%04x = (ACC)%04x => %x", self.accumulator, old, distance))
                    default:
                        throw IBCMError.invalidOpcode
                }
            case 0x3: // load
                self.accumulator = self.memory[Int(addr)]
                Swift.print(String(format: "load  (ACC)%04x = [%03x]%04x", self.accumulator, addr, self.accumulator))
            case 0x4: // store
                Swift.print(String(format: "store [%03x]%04x = (ACC)%04x", addr, self.accumulator, self.accumulator))
                self.memory[Int(addr)] = self.accumulator
            case 0x5: // add
                let res = self.accumulator &+ self.memory[Int(addr)]
                Swift.print(String(format: "add   (ACC)%04x = (ACC)%04x + [%03x]%04x", res, self.accumulator, addr, self.memory[Int(addr)]))
                self.accumulator = res
            case 0x6: // sub
                let res = self.accumulator &- self.memory[Int(addr)]
                Swift.print(String(format: "sub   (ACC)%04x = (ACC)%04x - [%03x]%04x", res, self.accumulator, addr, self.memory[Int(addr)]))
                self.accumulator = res
            case 0x7: // and
                let res = self.accumulator & self.memory[Int(addr)]
                Swift.print(String(format: "and   (ACC)%04x = (ACC)%04x & [%03x]%04x", res, self.accumulator, addr, self.memory[Int(addr)]))
                self.accumulator = res
            case 0x8: // or
                let res = self.accumulator | self.memory[Int(addr)]
                Swift.print(String(format: "or    (ACC)%04x = (ACC)%04x | [%03x]%04x", res, self.accumulator, addr, self.memory[Int(addr)]))
                self.accumulator = res
            case 0x9: // xor
                let res = self.accumulator ^ self.memory[Int(addr)]
                Swift.print(String(format: "xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x", res, self.accumulator, addr, self.memory[Int(addr)]))
                self.accumulator = res
            case 0xA: // not
                let res = ~self.accumulator
                Swift.print(String(format: "not   (ACC)%04x = ! (ACC)%04x", res, self.accumulator, addr))
                self.accumulator = res
            case 0xB: // nop
                Swift.print("nop")
            case 0xC: // jmp
                Swift.print(String(format: "jmp   [%03x]", addr))
                self.pc = addr
                return
            case 0xD: // jmpe
                if self.accumulator == 0 {
                    Swift.print(String(format: "jmpe  [%03x]", addr))
                    self.pc = addr
                    return
                } else {
                    Swift.print(String(format: "jmpe  (ACC)%04x", self.accumulator))
                }
            case 0xE: // jmpl
                if self.accumulator & 0x8000 != 0 {
                    Swift.print(String(format: "jmpl  [%03x]", addr))
                    self.pc = addr
                    return
                } else {
                    Swift.print(String(format: "jmpl  (ACC)%04x", self.accumulator))
                }
            case 0xF: // brl
                self.accumulator = self.pc + 1;
                Swift.print(String(format: "brl   [%03x]  (ACC)%04x", addr, self.accumulator));
                self.pc = addr
                return
            default:
                throw IBCMError.invalidOpcode
        }
        self.pc += 1
        if self.pc >= MEM_SIZE {
            throw IBCMError.memoryOverflow
        }
    }

    mutating func run() throws {
        while true {
            try self.step()
            if self.halted {
                break
            }
        }
    }
}
