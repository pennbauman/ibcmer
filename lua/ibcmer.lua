#!/usr/bin/env lua
-- IBCMer - Lua
--   URL: https://github.com/pennbauman/ibcmer
--   License: Creative Commons Attribution Share Alike 4.0 International
--   Author: Penn Bauman <me@pennbauman.com>
MEM_SIZE = 4096

-- Check if a string is all hex and between min and max length
function checkhex(str, min, max)
	if min then
		if string.len(str) < min then
			return false
		end
	end
	if max then
		if string.len(str) > max then
			return false
		end
	end

	local i = 1
	while i <= string.len(str) do
		if tonumber(string.sub(str, i, i), 16) == nil then
			return false
		end
		i = i + 1
	end
	return true
end

-- Check if file exists
function checkfile(path)
	local f = io.open(path, "r")
	if f ~= nil then
	   io.close(f)
	   return true
	else
	   return false
	end
end


-- Check code file argument
if (#arg < 1) then
	io.stderr:write("Error: Missing code file\n")
	os.exit(1)
end


-- Parse code file
MEM = {}
i = 0
if checkfile(arg[1]) then
	for line in io.lines(arg[1]) do
		if i >= MEM_SIZE then
			io.stderr:write(string.format("Error: Code file overflows memory (%d lines max)\n", MEM_SIZE))
			os.exit(1)
		end
		opcode = string.sub(line, 1, 4)
		if not checkhex(opcode, 4, 4) then
			j = 1
			while checkhex(string.sub(line, j, j), 1, 1) do
				j = j + 1
			end
			io.stderr:write(string.format("Error: '%s:%d:%d' Invalid opcode hexadecimal\n\n    %s\n    ", arg[1], i + 1, j, line))
			io.stderr:write(string.format(string.format("%%%ds\n", j), "^"))
			os.exit(1)
		end
		MEM[i] = tonumber(opcode, 16)
		if MEM[i] == nil then
			io.stderr:write(string.format("Error: Failed to parse '%s' to int\n", opcode))
			os.exit(1)
		end
		i = i + 1
	end
else
	io.stderr:write(string.format("Error: Code file '%s' not found\n", arg[1]))
	os.exit(1)
end
-- Fill rest of memory
while i < MEM_SIZE do
	MEM[i] = 0
	i = i + 1
end

ACC = 0
PC = 0

-- Execute code
while true do
	opcode = MEM[PC] >> 12
	address = MEM[PC] & 0xfff
	io.write(string.format("[%03x]%04x  ", PC, MEM[PC]))

	-- halt
	if opcode == 0x0 then
		print("halt")
		break
	-- i/o
	elseif opcode == 0x1 then
		subopcode = address >> 8
		print(string.format("i/o   (ACC)%04x", ACC))
		if subopcode == 0x0 then
			io.write("Input hex:  ")
			input = io.read("*line")
			if not checkhex(input, 1, 4) then
				print(string.format("Invalid hex input '%s'", input))
				os.exit(2)
			end
			ACC = tonumber(input, 16)
		elseif subopcode == 0x4 then
			io.write("Input char: ")
			input = io.read("*line")
			if string.len(input) ~= 1 then
				print("A single character must be entered")
				os.exit(2)
			end
			ACC = string.byte(input)
		elseif subopcode == 0x8 then
			print(string.format("Output hex:  %04x", ACC))
		elseif subopcode == 0xC then
			print(string.format("Output char: %c", ACC))
		else
			print("")
			io.stderr:write(string.format("Error: Invalid I/O sub-opcode '%x'\n", subopcode))
			os.exit(2)
		end
	-- shift
	elseif opcode == 0x2 then
		subopcode = address >> 8
		distance = address & 0xf
		if subopcode == 0x0 then
			res = (ACC << distance) & 0xffff
			arrow = "<<"
		elseif subopcode == 0x4 then
			res = ACC >> distance
			arrow = ">>"
		elseif subopcode == 0x8 then
			res = ((ACC << distance) | (ACC >> (16 - distance))) & 0xffff
			arrow = "<="
		elseif subopcode == 0xC then
			res = ((ACC >> distance) | (ACC << (16 - distance))) & 0xffff
			arrow = "=>"
		else
			print("shift ")
			io.stderr:write(string.format("Error: Invalid shift sub-opcode '%x'\n", subopcode))
			os.exit(2)
		end
		print(string.format("shift (ACC)%04x = (ACC)%04x %s %x", res, ACC, arrow, distance))
		ACC = res
	-- load
	elseif opcode == 0x3 then
		ACC = MEM[address]
		print(string.format("load  (ACC)%04x = [%03x]%04x", ACC, address, ACC))
	-- store
	elseif opcode == 0x4 then
		MEM[address] = ACC
		print(string.format("store [%03x]%04x = (ACC)%04x", address, ACC, ACC))
	-- add
	elseif opcode == 0x5 then
		val = MEM[address]
		res = (ACC + val) & 0xffff
		print(string.format("add   (ACC)%04x = (ACC)%04x + [%03x]%04x", res, ACC, address, val))
		ACC = res
	-- sub
	elseif opcode == 0x6 then
		val = MEM[address]
		res = ACC - val
		if res < 0 then
			res = res + 0x10000
		end
		print(string.format("sub   (ACC)%04x = (ACC)%04x - [%03x]%04x", res, ACC, address, val))
		ACC = res
	-- and
	elseif opcode == 0x7 then
		val = MEM[address]
		res = ACC & val
		print(string.format("and   (ACC)%04x = (ACC)%04x & [%03x]%04x", res, ACC, address, val))
		ACC = res
	-- or
	elseif opcode == 0x8 then
		val = MEM[address]
		res = ACC | val
		print(string.format("or    (ACC)%04x = (ACC)%04x | [%03x]%04x", res, ACC, address, val))
		ACC = res
	-- xor
	elseif opcode == 0x9 then
		val = MEM[address]
		res = ACC ~ val
		print(string.format("xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x", res, ACC, address, val))
		ACC = res
	-- not
	elseif opcode == 0xA then
		res = ~ACC & 0xffff
		print(string.format("not   (ACC)%04x = ! (ACC)%04x", res, ACC))
		ACC = res
	-- nop
	elseif opcode == 0xB then
		print("nop")
	-- jmp
	elseif opcode == 0xC then
		print(string.format("jmp   [%03x]", address))
		PC = address - 1
	-- jmpe
	elseif opcode == 0xD then
		if ACC == 0 then
			print(string.format("jmpe  [%03x]", address))
			PC = address - 1
		else
			print(string.format("jmpe  (ACC)%04x", ACC))
		end
	-- jmpl
	elseif opcode == 0xE then
		if ACC >> 15 == 1 then
			print(string.format("jmpl  [%03x]", address))
			PC = address - 1
		else
			print(string.format("jmpl  (ACC)%04x", ACC))
		end
	--  brl
	elseif opcode == 0xF then
		ACC = PC + 1
		PC = address - 1
		print(string.format("brl   [%03x]  (ACC)%04x", address, ACC))
	else
		io.stderr:write(string.format("Error: Unknown opcode '%04x\n'", opcode))
		os.exit(2)
	end
	PC = PC + 1

	if PC > 0xfff then
		io.stderr:write(string.format("Error: Memory overflow (PC = 0x%04x)\n", PC))
		os.exit(3)
	end
end
