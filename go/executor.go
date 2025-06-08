// ICBMer - Go Library
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
package main

import (
	"bufio"
	"errors"
	"fmt"
	"os"
	"strconv"
	"strings"
)

const MEM_SIZE = 4096

type Executor struct {
	Acc uint16
	PC  uint16
	Mem [MEM_SIZE]uint16

	Halted bool
	Logging bool
}

func (e Executor) WithLogging(settings bool) Executor {
	e.Logging = settings
	return e
}

func (e Executor) FromFile(path string) (Executor, error) {
	var file, err = os.OpenFile(path, os.O_RDONLY, 0644)
	if err != nil {
		fmt.Println(err)
		return e, err
	}
	scanner := bufio.NewScanner(file)

	scanner.Split(bufio.ScanLines)

	var addr uint64
	for scanner.Scan() {
		if addr >= MEM_SIZE {
			return e, errors.New(fmt.Sprintf("Code file overflows memory (%d lines max)", MEM_SIZE))
		}

		var line = scanner.Text()
		if len(line) < 4 {
			return e, errors.New(fmt.Sprintf("'%s:%d:%d' Invalid opcode hexadecimal\n\n    %s\n    %s^", path, addr + 1, len(line) + 1, line, strings.Repeat(" ", len(line))))
		}
		// Check hex opcode
		x, err := strconv.ParseUint(line[0:4], 16, 16)
		if err != nil {
			file.Close()
				// Find location of invalid hex
				var i = 0
				for i < 4 {
					_, err := strconv.ParseUint(line[i:i + 1], 16, 16)
					if err != nil {
						break;
					}
					i++;
				}
				return e, errors.New(fmt.Sprintf("'%s:%d:%d' Invalid opcode hexadecimal\n\n    %s\n    %s^", path, addr + 1, i + 1, line, strings.Repeat(" ", i)))
		}
		e.Mem[addr] = uint16(x)
		addr++
	}
	file.Close()
	return e, nil
}

func (e Executor) Print() {
	fmt.Println("IBCM -----")
	fmt.Printf("Acc = %04x\n", e.Acc)
	fmt.Printf("PC  = %04x\n", e.PC)
	end := MEM_SIZE
	for end > 0 {
		end -= 1
		if e.Mem[end] != 0 {
			break
		}
	}
	for i := 0; i < end; i++ {
		fmt.Printf("%03x = %04x\n", i, e.Mem[i])
	}
}

func (e Executor) Step() (Executor, error) {
	addr := e.Mem[e.PC] & 0x0fff
	switch e.Mem[e.PC] >> 12 {
	case 0x0:
		if e.Logging {
			fmt.Printf("[%03x]%04x  halt\n", e.PC, e.Mem[e.PC])
		}
		e.Halted = true
		return e, nil
	case 0x1:
		if e.Logging {
			fmt.Printf("[%03x]%04x  i/o   (ACC)%04x\n", e.PC, e.Mem[e.PC], e.Acc)
		}
		switch e.Mem[e.PC] & 0x0f00 {
		case 0x0000:
			var in string
			fmt.Print("Input hex:  ")
			fmt.Scanln(&in)
			x, err := strconv.ParseUint(in, 16, 16)
			if err != nil {
				return e, err
			}
			e.Acc = uint16(x)
		case 0x0400:
			var in string
			fmt.Print("Input char: ")
			fmt.Scanln(&in)
			if len(in) != 1 {
				return e, errors.New("Invalid character")
			}
			e.Acc = uint16(in[0])
		case 0x0800:
			fmt.Printf("Output hex:  %04x\n", e.Acc)
		case 0x0c00:
			fmt.Printf("Output char: %s\n", string(rune(e.Acc & 0x7f)))
		default:
			return e, errors.New("Invalid i/o type")
		}
	case 0x2:
		if e.Logging {
			fmt.Printf("[%03x]%04x  shift ", e.PC, e.Mem[e.PC])
		}
		distance := e.Mem[e.PC] & 0x000f
		var result uint16
		switch e.Mem[e.PC] & 0x0f00 {
		case 0x0000:
			result = e.Acc << distance
			if e.Logging {
				fmt.Printf("(ACC)%04x = (ACC)%04x << %x\n", result, e.Acc, distance)
			}
		case 0x0400:
			result = e.Acc >> distance
			if e.Logging {
				fmt.Printf("(ACC)%04x = (ACC)%04x >> %x\n", result, e.Acc, distance)
			}
		case 0x0800:
			result = (e.Acc << distance) | (e.Acc >> (0x10 - distance))
			if e.Logging {
				fmt.Printf("(ACC)%04x = (ACC)%04x <= %x\n", result, e.Acc, distance)
			}
		case 0x0c00:
			result = (e.Acc >> distance) | (e.Acc << (0x10 - distance))
			if e.Logging {
				fmt.Printf("(ACC)%04x = (ACC)%04x => %x\n", result, e.Acc, distance)
			}
		default:
			return e, errors.New("Invalid shift type")
		}
		e.Acc = result
	case 0x3:
		result := e.Mem[addr]
		if e.Logging {
			fmt.Printf("[%03x]%04x  load  (ACC)%04x = [%03x]%04x\n",
					e.PC, e.Mem[e.PC], result, addr, result)
		}
		e.Acc = result
	case 0x4:
		if e.Logging {
			fmt.Printf("[%03x]%04x  store [%03x]%04x = (ACC)%04x\n",
					e.PC, e.Mem[e.PC], addr, e.Acc, e.Acc)
		}
		e.Mem[addr] = e.Acc
	case 0x5:
		result := e.Acc + e.Mem[addr]
		if e.Logging {
			fmt.Printf("[%03x]%04x  add   (ACC)%04x = (ACC)%04x + [%03x]%04x\n",
					e.PC, e.Mem[e.PC], result, e.Acc, addr, e.Mem[addr])
		}
		e.Acc = result
	case 0x6:
		result := e.Acc - e.Mem[addr]
		if e.Logging {
			fmt.Printf("[%03x]%04x  sub   (ACC)%04x = (ACC)%04x - [%03x]%04x\n",
					e.PC, e.Mem[e.PC], result, e.Acc, addr, e.Mem[addr])
		}
		e.Acc = result
	case 0x7:
		result := e.Acc & e.Mem[addr]
		if e.Logging {
			fmt.Printf("[%03x]%04x  and   (ACC)%04x = (ACC)%04x & [%03x]%04x\n",
					e.PC, e.Mem[e.PC], result, e.Acc, addr, e.Mem[addr])
		}
		e.Acc = result
	case 0x8:
		result := e.Acc | e.Mem[addr]
		if e.Logging {
			fmt.Printf("[%03x]%04x  or    (ACC)%04x = (ACC)%04x | [%03x]%04x\n",
				e.PC, e.Mem[e.PC], result, e.Acc, addr, e.Mem[addr])
		}
		e.Acc = result
	case 0x9:
		result := e.Acc ^ e.Mem[addr]
		if e.Logging {
			fmt.Printf("[%03x]%04x  xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x\n",
					e.PC, e.Mem[e.PC], result, e.Acc, addr, e.Mem[addr])
		}
		e.Acc = result
	case 0xa:
		result := 0xffff ^ e.Acc
		if e.Logging {
			fmt.Printf("[%03x]%04x  not   (ACC)%04x = ! (ACC)%04x\n",
					e.PC, e.Mem[e.PC], result, e.Acc)
		}
		e.Acc = result
	case 0xb:
		if e.Logging {
			fmt.Printf("[%03x]%04x  nop\n", e.PC, e.Mem[e.PC])
		}
	case 0xc:
		if e.Logging {
			fmt.Printf("[%03x]%04x  jmp   [%03x]\n", e.PC, e.Mem[e.PC], addr)
		}
		e.PC = addr
		return e, nil
	case 0xd:
		if e.Logging {
			fmt.Printf("[%03x]%04x  jmpe  ", e.PC, e.Mem[e.PC])
		}
		if e.Acc == 0 {
			if e.Logging {
				fmt.Printf("[%03x]\n", addr)
			}
			e.PC = addr
			return e, nil
		} else {
			if e.Logging {
				fmt.Printf("(ACC)%04x\n", e.Acc)
			}
		}
	case 0xe:
		if e.Logging {
			fmt.Printf("[%03x]%04x  jmpl  ", e.PC, e.Mem[e.PC])
		}
		if e.Acc & 0x8000 != 0 {
			if e.Logging {
				fmt.Printf("[%03x]\n", addr)
			}
			e.PC = addr
			return e, nil
		} else {
			if e.Logging {
				fmt.Printf("(ACC)%04x\n", e.Acc)
			}
		}
	case 0xf:
		e.Acc = e.PC + 1
		if e.Logging {
			fmt.Printf("[%03x]%04x  brl   [%03x]  (ACC)%04x\n", e.PC, e.Mem[e.PC], addr, e.Acc)
		}
		e.PC = addr
		return e, nil
	}
	e.PC += 1
	if e.PC >= MEM_SIZE {
		return e, errors.New(fmt.Sprintf("Memory overflow (PC = 0x%04x)", e.PC))
	}
	return e, nil
}

func (e Executor) Run() error {
	state := e
	for true {
		s, err := state.Step()
		if err != nil {
			return err
		}
		state = s
		if state.Halted {
			break
		}
	}
	return nil
}
