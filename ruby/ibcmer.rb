#!/usr/bin/ruby
# IBCMer - Ruby
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
MEM_SIZE = 4096


def checkhex(str, min)
  if str.length < min or str.length > 4
    return false
  end
  begin
    return Integer("0x" + str.chomp)
  rescue
    return false
  end
  return true
end


$MEM = Array.new(4096, 0)
$ACC = 0
$PC = 0


# Read code file
if ARGV.length < 1
  STDERR.puts "Missing code file"
  exit(1)
else
  i = 0
  if not File.file?(ARGV[0])
    STDERR.puts "File not found '" + ARGV[0] + "'"
    exit(1)
  end

  File.open(ARGV[0]).each do |line|
    if checkhex(line[0..3], 4)
      $MEM[i] = Integer("0x" + line[0..3])
    else
      STDERR.puts "Invalid line '" + line[0..3] + "'"
      exit(1)
    end
    i += 1
  end
end


# Execute code
loop do
  opcode = $MEM[$PC] >> 12
  address = $MEM[$PC] & 0xfff
  print "[%03x]%04x  " % [$PC, $MEM[$PC]]

  case opcode
  # halt
  when 0x0
    puts "halt"
    break
  # i/o
  when 0x1
    subopcode = address >> 8
    puts "i/o   (ACC)%04x" % $ACC
    case subopcode
    when 0x0
      print "Input hex:  "
      input = STDIN.gets.chomp
      if checkhex(input, 1)
        $ACC = Integer("0x" + input)
      else
        puts "Invalid hex input '%s'" % input
        exit(2)
      end
    when 0x4
      print "Input char: "
      input = STDIN.gets.chomp
      if input.length != 1
        puts "A single character must be entered"
        exit(2)
      end
      $ACC = input.ord
    when 0x8
      puts "Output hex:  %04x" % $ACC
    when 0xC
      puts "Output char: %c" % $ACC
    else
      STDERR.puts "Unknown i/o sub-opcode '%x'" % subopcode
      exit(2)
    end
  # shift
  when 0x2
    subopcode = address >> 8
    distance = address & 0xf
    case subopcode
    when 0x0
      res = ($ACC << distance) & 0xffff
      arrow = "<<"
    when 0x4
      res = $ACC >> distance
      arrow = ">>"
    when 0x8
      res = (($ACC << distance) | ($ACC >> (16 - distance))) & 0xffff
      arrow = "<="
    when 0xC
      res = (($ACC >> distance) | ($ACC << (16 - distance))) & 0xffff;
      arrow = "=>";
    else
      STDERR.puts "Unknown shift sub-opcode '%x'" % subopcode
      exit(2)
    end
    puts "shift (ACC)%04x = (ACC)%04x %s %x" % [res, $ACC, arrow, distance]
    $ACC = res
  # load
  when 0x3
    $ACC = $MEM[address]
    puts "load  (ACC)%04x = [%03x]%04x" % [$ACC, address, $ACC]
  # store
  when 0x4
    $MEM[address] = $ACC
    puts "store [%03x]%04x = (ACC)%04x" % [address, $ACC, $ACC]
  # add
  when 0x5
    val = $MEM[address]
    res = ($ACC + val) & 0xffff
    puts "add   (ACC)%04x = (ACC)%04x + [%03x]%04x" % [res, $ACC, address, val]
    $ACC = res
  # sub
  when 0x6
    val = $MEM[address]
    res = $ACC - val
    if res < 0 then
      res = res + 0x10000
    end
    puts "sub   (ACC)%04x = (ACC)%04x - [%03x]%04x" % [res, $ACC, address, val]
    $ACC = res
  # and
  when 0x7
    val = $MEM[address]
    res = $ACC & val
    puts "and   (ACC)%04x = (ACC)%04x & [%03x]%04x" % [res, $ACC, address, val]
    $ACC = res
  # or
  when 0x8
    val = $MEM[address]
    res = $ACC | val
    puts "or    (ACC)%04x = (ACC)%04x | [%03x]%04x" % [res, $ACC, address, val]
    $ACC = res
  # xor
  when 0x9
    val = $MEM[address]
    res = $ACC ^ val
    puts "xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x" % [res, $ACC, address, val]
    $ACC = res
  # not
  when 0xA
    res = ~$ACC & 0xffff
    puts "not   (ACC)%04x = ! (ACC)%04x" % [res, $ACC]
    $ACC = res
  # nop
  when 0xB
    puts "nop"
  # jmp
  when 0xC
    puts "jmp   [%03x]" % address
    $PC = address - 1
  # jmpe
  when 0xD
    if $ACC == 0
      puts "jmpe  [%03x]" % address
      $PC = address - 1
    else
      puts "jmpe  (ACC)%04x" % $ACC
    end
  # jmpl
  when 0xE
    if $ACC >> 15 == 1
      puts "jmpl  [%03x]" % address
      $PC = address - 1
    else
      puts "jmpl  (ACC)%04x" % $ACC
    end
  # brl
  when 0xF
    $ACC = $PC + 1
    $PC = address - 1
    puts "brl   [%03x]  (ACC)%04x" % [address, $ACC]
  else
    STDERR.puts "Unknown opcode '%04x'" % opcode
    os.exit(2)
  end

  $PC += 1
  if $PC > 0xfff
    STDERR.puts "PC overflow"
    exit(3)
  end
end
