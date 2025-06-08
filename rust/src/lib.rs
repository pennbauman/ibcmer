// IBCMer - Rust Library
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
use std::char;
use std::fs::File;
use std::io;
use std::io::{Write, BufRead, BufReader};


pub const MEM_SIZE: usize = 4096;
const PRINT_COLUMNS: usize = 8;


#[derive(Debug)]
pub struct IttyBittyComputingMachine {
    pc: u16,
    accumulator: u16,
    memory: [u16; MEM_SIZE],

    logging: bool,
    halted: bool,
}
impl IttyBittyComputingMachine {
    pub fn new() -> Self {
        Self {
            accumulator: 0,
            pc: 0,
            memory: [0; MEM_SIZE],
            logging: true,
            halted: false,
        }
    }
    pub fn from_file<S: ToString>(name: S) -> Result<Self, Error> {
        let file = File::open(name.to_string()).unwrap();
        let reader = BufReader::new(file);
        let mut fin = Self::new();
        let mut addr = 0;
        for l in reader.lines() {
            if addr >= MEM_SIZE {
                return Err(Error::CodeFileOverflow)
            }
            let line = l?;
            if line.len() < 4 {
                let len = line.len();
                return Err(Error::InvalidHex {
                    file_name: name.to_string(),
                    line_text: line,
                    line_num: addr,
                    char_idx: len
                });
            }
            let code = &line[0..4];
            let mut i = 0;
            for c in code.chars() {
                if !c.is_digit(16) {
                    return Err(Error::InvalidHex {
                        file_name: name.to_string(),
                        line_text: line,
                        line_num: addr,
                        char_idx: i
                    });
                }
                i += 1;
            }
            let hex = match u16::from_str_radix(code, 16) {
                Ok(n) => n,
                Err(_) => return Err(Error::InvalidHex {
                    file_name: name.to_string(),
                    line_text: line,
                    line_num: addr,
                    char_idx: 6
                }),
            };
            fin.memory[addr] = hex;
            addr += 1;
        }
        return Ok(fin);
    }

    pub fn unhalted(&self) -> bool {
        !self.halted
    }

    fn step(&mut self) -> Result<(), Error> {
        if self.pc as usize >= MEM_SIZE {
            return Err(Error::MemoryOverflow(self.pc));
        }
        let op = self.memory[self.pc as usize];
        let addr = (op & 0x0fff) as usize;

        // println!("{:04x} : {:03x}", op, addr);
        match op >> 12 {
            0x0 => {
                self.halted = true;
                self.log("halt");
                return Ok(());
            },
            0x1 => {
                self.log(format!("i/o   (ACC){:04x}", self.accumulator));
                match op & 0x0f00 {
                    0x0000 => {
                        print!("Input hex:  ");
                        io::stdout().flush().unwrap();
                        let mut line = String::new();
                        std::io::stdin().read_line(&mut line)?;
                        let hex = match u16::from_str_radix(line.trim(), 16) {
                            Ok(n) => n,
                            Err(_) => return Err(Error::InvalidInput(line.trim().to_string())),
                        };
                        self.accumulator = hex;
                    },
                    0x0400 => {
                        print!("Input char: ");
                        io::stdout().flush().unwrap();
                        let mut line = String::new();
                        std::io::stdin().read_line(&mut line)?;
                        if line.trim_end_matches("\n").len() != 1 {
                            return Err(Error::InvalidInput(line.trim().to_string()));
                        }
                        let c = match line.trim_end_matches("\n").chars().next() {
                            Some(n) => n,
                            None => return Err(Error::InvalidInput(line.trim().to_string())),
                        };
                        let mut buf = [0; 8];
                        let result = c.encode_utf16(&mut buf);
                        if result.len() != 1 {
                            return Err(Error::InvalidInput(line.trim().to_string()));
                        }
                        self.accumulator = buf[0];
                    },
                    0x0800 => {
                        println!("Output hex:  {:04x}", self.accumulator);
                    },
                    0x0c00 => {
                        println!("Output char: {}", char::from_u32(self.accumulator as u32).unwrap_or('\0'));
                    },
                    _ => return Err(Error::IllegalOp(format!("{:04x}", op))),
                }
            },
            0x2 => {
                let old = self.accumulator;
                let distance = op & 0x000f;
                match op & 0x0f00 {
                    0x0000 => {
                        self.accumulator = self.accumulator << distance;
                        self.log(format!("shift (ACC){:04x} = (ACC){:04x} << {:x}", self.accumulator, old, distance));
                    },
                    0x0400 => {
                        self.accumulator = self.accumulator >> distance;
                        self.log(format!("shift (ACC){:04x} = (ACC){:04x} >> {:x}", self.accumulator, old, distance));
                    },
                    0x0800 => {
                        self.accumulator = self.accumulator.rotate_left(distance as u32);
                        self.log(format!("shift (ACC){:04x} = (ACC){:04x} <= {:x}", self.accumulator, old, distance));
                    },
                    0x0c00 => {
                        self.accumulator = self.accumulator.rotate_right(distance as u32);
                        self.log(format!("shift (ACC){:04x} = (ACC){:04x} => {:x}", self.accumulator, old, distance));
                    },
                    _ => return Err(Error::IllegalOp(format!("{:04x}", op))),
                }
            },
            0x3 => {
                self.accumulator = self.memory[addr];
                self.log(format!("load  (ACC){:04x} = [{:03x}]{:04x}", self.accumulator, addr, self.accumulator));
            },
            0x4 => {
                self.log(format!("store [{:03x}]{:04x} = (ACC){:04x}", addr, self.accumulator, self.accumulator));
                self.memory[addr] = self.accumulator;
            },
            0x5 => {
                let result = self.accumulator.wrapping_add(self.memory[addr]);
                self.log(format!("add   (ACC){:04x} = (ACC){:04x} + [{:03x}]{:04x}", result, self.accumulator, addr, self.memory[addr]));
                self.accumulator = result;
            },
            0x6 => {
                let result = self.accumulator.wrapping_sub(self.memory[addr]);
                self.log(format!("sub   (ACC){:04x} = (ACC){:04x} - [{:03x}]{:04x}", result, self.accumulator, addr, self.memory[addr]));
                self.accumulator = result;
            },
            0x7 => {
                let result = self.accumulator & self.memory[addr];
                self.log(format!("and   (ACC){:04x} = (ACC){:04x} & [{:03x}]{:04x}", result, self.accumulator, addr, self.memory[addr]));
                self.accumulator = result;
            },
            0x8 => {
                let result = self.accumulator | self.memory[addr];
                self.log(format!("or    (ACC){:04x} = (ACC){:04x} | [{:03x}]{:04x}", result, self.accumulator, addr, self.memory[addr]));
                self.accumulator = result;
            },
            0x9 => {
                let result = self.accumulator ^ self.memory[addr];
                self.log(format!("xor   (ACC){:04x} = (ACC){:04x} ^ [{:03x}]{:04x}", result, self.accumulator, addr, self.memory[addr]));
                self.accumulator = result;
            },
            0xa => {
                let result = !self.accumulator;
                self.log(format!("not   (ACC){:04x} = ! (ACC){:04x}", result, self.accumulator));
                self.accumulator = result;
            },
            0xb => {
                self.log("nop");
            },
            0xc => {
                self.log(format!("jmp   [{:03x}]", addr));
                self.pc = addr as u16;
                return Ok(());
            },
            0xd => {
                if self.accumulator == 0 {
                    self.log(format!("jmpe  [{:03x}]", addr));
                    self.pc = addr as u16;
                    return Ok(());
                } else {
                    self.log(format!("jmpe  (ACC){:04x}", self.accumulator));
                }
            },
            0xe => {
                if self.accumulator.leading_ones() > 0 {
                    self.log(format!("jmpl  [{:03x}]", addr));
                    self.pc = addr as u16;
                    return Ok(());
                } else {
                    self.log(format!("jmpl  (ACC){:04x}", self.accumulator));
                }
            },
            0xf => {
                self.accumulator = self.pc + 1;
                self.log(format!("brl   [{:03x}]  (ACC){:04x}", addr, self.accumulator));
                self.pc = addr as u16;
                return Ok(());
            },
            _ => panic!("Impossible op code"),
        };
        self.pc += 1;
        return Ok(());
    }
    fn log<S: ToString>(&self, message: S) {
        if self.logging {
            println!("[{:03x}]{:04x}  {}", self.pc, self.memory[self.pc as usize], message.to_string());
        }
    }
    pub fn set_logging(&mut self, enabled: bool) {
        self.logging = enabled;
    }

    pub fn run(&mut self) -> Result<(), Error> {
        while self.unhalted() {
            // println!("{:?}", self.step());
            self.step()?;
        }
        Ok(())
    }
    pub fn print(&self) {
        println!("IBCM:      ACC: {:04x}   PC: {:03x}", self.accumulator, self.pc);
        let mut last = MEM_SIZE - 1;
        while self.memory[last] == 0 {
            if last == 0 {
                break;
            }
            last -= 1;
        }
        let mut column = 0;
        for addr in 0..MEM_SIZE {
            print!("[{:03x}]{:04x}  ", addr, self.memory[addr]);
            column += 1;
            if column == PRINT_COLUMNS {
                println!("");
                if addr >= last {
                    break;
                }
                column = 0;
            }
        }
    }
}


#[derive(thiserror::Error, Debug)]
pub enum Error {
    #[error("IO Error")]
    IO(#[from] std::io::Error),
    #[error("Invalid input '{0}'")]
    InvalidInput(String),
    #[error("'{file_name}:{0}:{1}' Invalid opcode hexadecimal\n\n    {line_text}\n    {2:char_idx$}^", line_num + 1, char_idx + 1, "")]
    InvalidHex{
        file_name: String,
        line_text: String,
        line_num: usize,
        char_idx: usize
    },
    #[error("Code file overflows memory ({0} lines max)", MEM_SIZE)]
    CodeFileOverflow,
    #[error("Illegal operation '{0}'")]
    IllegalOp(String),
    #[error("Memory overflow (PC = 0x{0:04x})")]
    MemoryOverflow(u16),
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn loading_empty_file() {
        let machine = IttyBittyComputingMachine::from_file("tests/empty-file.ibcm");
        assert!(machine.is_ok());
        assert!(machine.unwrap().memory[0] == 0x0000);
    }
    #[test]
    fn loading_nop_file() {
        let machine = IttyBittyComputingMachine::from_file("tests/nop.ibcm");
        assert!(machine.is_ok());
        assert!(machine.unwrap().memory[0] == 0xb000);
    }
    #[test]
    fn loading_empty_line() {
        let machine = IttyBittyComputingMachine::from_file("tests/empty-line.ibcm");
        assert!(machine.is_err());
        assert!(match machine.unwrap_err() {
            Error::InvalidHex { file_name: _, line_text: _, line_num: _, char_idx: _ } => true,
            _ => false
        });
    }
    #[test]
    fn loading_nonhex_op() {
        let machine = IttyBittyComputingMachine::from_file("tests/nonhex-op.ibcm");
        assert!(machine.is_err());
        assert!(match machine.unwrap_err() {
            Error::InvalidHex { file_name: _, line_text: _, line_num: _, char_idx: _ } => true,
            _ => false
        });
    }
    #[test]
    fn loading_short_op() {
        let machine = IttyBittyComputingMachine::from_file("tests/short-op.ibcm");
        assert!(machine.is_err());
        assert!(match machine.unwrap_err() {
            Error::InvalidHex { file_name: _, line_text: _, line_num: _, char_idx: _ } => true,
            _ => false
        });
    }

    // 0 halt
    #[test]
    fn opcode_halt() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.step().unwrap();
        assert!(ibcm.halted);
        assert!(ibcm.accumulator == 0);
        assert!(ibcm.pc == 0);
    }
    // 1 i/o
    // #[test]
    // fn opcode_io_read_hex() {
    //     todo!();
    // }
    // #[test]
    // fn opcode_io_read_ascii() {
    //     todo!();
    // }
    #[test]
    fn opcode_io_output_hex() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0xabcd;
        ibcm.memory[0] = 0x1800;
        assert!(ibcm.step().is_ok());
    }
    #[test]
    fn opcode_io_output_ascii() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0xabcd;
        ibcm.memory[0] = 0x1c00;
        assert!(ibcm.step().is_ok());
    }
    #[test]
    fn opcode_io_err() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0x1777;
        assert!(ibcm.step().is_err());
    }
    // 2 shift
    #[test]
    fn opcode_shift_left() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0x2003;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b1001_1000_0111_1000);
    }
    #[test]
    fn opcode_shift_right() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0x240b;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b0000_0000_0000_1010);
    }
    #[test]
    fn opcode_rotate_left() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0x2808;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b0000_1111_0101_0011);
    }
    #[test]
    fn opcode_rotate_right() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0x2c05;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b0111_1010_1001_1000);
    }
    #[test]
    fn opcode_shift_err() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0x2777;
        assert!(ibcm.step().is_err());
    }
    // 3 load
    #[test]
    fn opcode_load() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0x3963;
        ibcm.memory[0x963] = 0x1234;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0x1234);
    }
    // 4 store
    #[test]
    fn opcode_store() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0x5678;
        ibcm.memory[0] = 0x4123;
        ibcm.step().unwrap();
        assert!(ibcm.memory[0x123] == 0x5678);
    }
    // 5 add
    #[test]
    fn opcode_add() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0x0002;
        ibcm.memory[0] = 0x5111;
        ibcm.memory[0x111] = 0xabcd;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0xabcf);
        ibcm.memory[1] = 0x500f;
        ibcm.memory[0x00f] = 0x9999;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0x4568);
    }
    // 6 sub
    #[test]
    fn opcode_sub() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0x0165;
        ibcm.memory[0] = 0x60a1;
        ibcm.memory[0x0a1] = 0x0043;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0x0122);
        ibcm.memory[1] = 0x60a2;
        ibcm.memory[0x0a2] = 0x9999;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0x6789);
    }
    // 7 and
    #[test]
    fn opcode_and() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0x70a1;
        ibcm.memory[0x0a1] = 0b1010_0101_0011_0011;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b0000_0001_0000_0011);
    }
    // 8 or
    #[test]
    fn opcode_or() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0x80a1;
        ibcm.memory[0x0a1] = 0b1010_0101_0011_0011;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b1111_0111_0011_1111);
    }
    // 9 xor
    #[test]
    fn opcode_xor() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0x90a1;
        ibcm.memory[0x0a1] = 0b1010_0101_0011_0011;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b1111_0110_0011_1100);
    }
    // A not
    #[test]
    fn opcode_not() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.accumulator = 0b0101_0011_0000_1111;
        ibcm.memory[0] = 0xa000;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0b1010_1100_1111_0000);
    }
    // B nop
    #[test]
    fn opcode_nop() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0xb000;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0);
        for i in 1..MEM_SIZE {
            assert!(ibcm.memory[i] == 0);
        }
    }
    // C jmp
    #[test]
    fn opcode_jmp() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0xc345;
        ibcm.step().unwrap();
        assert!(ibcm.pc == 0x345);
    }
    // D jmpe
    #[test]
    fn opcode_jmpe() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0xd123;
        ibcm.accumulator = 0xffff;
        ibcm.step().unwrap();
        assert!(ibcm.pc == 0x1);
        ibcm.memory[1] = 0xd456;
        ibcm.accumulator = 0;
        ibcm.step().unwrap();
        assert!(ibcm.pc == 0x456);
    }
    // E jmpl
    #[test]
    fn opcode_jmpl() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0xe123;
        ibcm.accumulator = 0x0022;
        ibcm.step().unwrap();
        assert!(ibcm.pc == 0x1);
        ibcm.memory[1] = 0xe456;
        ibcm.accumulator = 0x8000;
        ibcm.step().unwrap();
        assert!(ibcm.pc == 0x456);
    }
    // E jmpl
    #[test]
    fn opcode_brl() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.memory[0] = 0xfabc;
        ibcm.step().unwrap();
        assert!(ibcm.accumulator == 0x1);
        assert!(ibcm.pc == 0xabc);
    }

    #[test]
    fn memory_overflow() {
        let mut ibcm = IttyBittyComputingMachine::new();
        ibcm.pc = MEM_SIZE as u16;
        assert!(ibcm.step().is_err());
    }
}
