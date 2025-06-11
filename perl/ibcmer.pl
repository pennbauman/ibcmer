#!/usr/bin/perl
# IBCMer - Perl
#   URL: https://github.com/pennbauman/ibcmer
#   License: Creative Commons Attribution Share Alike 4.0 International
#   Author: Penn Bauman <me@pennbauman.com>
use strict;
use warnings;

my $MEM_SIZE = 4096;
my $PC = 0;
my $ACC = 0;
my @MEM = ();

# Check arguments
if ($#ARGV < 0) {
	print STDERR "Error: A code file must be provided\n";
	exit 1;
}
my $filename = $ARGV[0];
if (! -f $filename) {
	print STDERR "Error: Code file '$filename' not found\n";
	exit 1;
}

# Load memory from file
my $i = 0;
open(my $fh, "<", $filename) or die "Can't open file: $!";
while (my $line = readline($fh)) {
	if ($i >= $MEM_SIZE) {
		print STDERR "Error: Code file overflows memory ($MEM_SIZE lines max)\n";
		exit 1;
	}
	if ($line =~ /^([0-9a-fA-F]{4})/) {
		# print "$1\n";
		push(@MEM, hex($1));
	} else {
		$i = $i + 1;
		my $j = 0;
		while ((substr $line, $j, 1) =~ /^[0-9a-fA-F]/) {
			$j = $j + 1;
		}
		$j = $j + 1;
		print STDERR "Error: '$filename:$i:$j' Invalid opcode hexadecimal\n";
		print STDERR "\n    $line";
		print STDERR " " x $j;
		print STDERR "   ^\n";
		exit 1;
	}
	$i = $i + 1;
}
while ($i <= $MEM_SIZE) {
	push(@MEM, 0);
	$i = $i + 1;
}


while (1) {
	my $opcode = $MEM[$PC] >> 12;
	my $address = $MEM[$PC] & 0xfff;
	printf("[%03x]%04x  ", $PC, $MEM[$PC]);

	# halt
	if ($opcode == 0x0) {
		print "halt\n";
		exit 0;
	# i/o
	} elsif ($opcode == 0x1) {
		printf("i/o   (ACC)%04x\n", $ACC);
		my $subopcode = $address >> 8;
		if ($subopcode == 0x0) {
			print "Input hex:  ";
			my $in = <STDIN>;
			if ($in =~ /^([0-9a-fA-F]{1,4})$/) {
				$ACC = hex $1;
			} else {
				die "Invalid hex input '$in'";
			}
		} elsif ($subopcode == 0x4) {
			print "Input char: ";
			my $in = <STDIN>;
			if ($in =~ /^(.)$/) {
				$ACC = ord($1);
			} else {
				die "Invalid char input '$in'";
			}
		} elsif ($subopcode == 0x8) {
			printf("Output hex:  %04x\n", $ACC);
		} elsif ($subopcode == 0xC) {
			printf("Output char: %c\n", $ACC);
		} else {
			print "\n";
			print STDERR sprintf("Error: Invalid I/O sub-opcode '%x'\n", $subopcode);
			exit 1;
		}
	# shift
	} elsif ($opcode == 0x2) {
		my $subopcode = $address >> 8;
		my $distance = $address & 0xf;
		my $arrow = "";
		my $res = 0;
		if ($subopcode == 0x0) {
			$res = ($ACC << $distance) & 0xffff;
			$arrow = "<<";
		} elsif ($subopcode == 0x4) {
			$res = $ACC >> $distance;
			$arrow = ">>";
		} elsif ($subopcode == 0x8) {
			$res = (($ACC << $distance) | ($ACC >> (16 - $distance))) & 0xffff;
			$arrow = "<=";
		} elsif ($subopcode == 0xC) {
			$res = (($ACC >> $distance) | ($ACC << (16 - $distance))) & 0xffff;
			$arrow = "=>";
		} else {
			print "shift \n";
			print STDERR sprintf("Error: Invalid shift sub-opcode '%x'\n", $subopcode);
			exit 1;
		}
		printf("shift (ACC)%04x = (ACC)%04x %s %x\n", $res, $ACC, $arrow, $distance);
		$ACC = $res;
	# load
	} elsif ($opcode == 0x3) {
		my $val = $MEM[$address];
		printf("load  (ACC)%04x = [%03x]%04x\n", $val, $address, $val);
		$ACC = $val;
	# store
	} elsif ($opcode == 0x4) {
		printf("store [%03x]%04x = (ACC)%04x\n", $address, $ACC, $ACC);
		$MEM[$address] = $ACC;
	# add
	} elsif ($opcode == 0x5) {
		my $val = $MEM[$address];
		my $res = ($ACC + $val) & 0xffff;
		printf("add   (ACC)%04x = (ACC)%04x + [%03x]%04x\n", $res, $ACC, $address, $val);
		$ACC = $res;
	# sub
	} elsif ($opcode == 0x6) {
		my $val = $MEM[$address];
		my $res = $ACC - $val;
		if ($res < 0) {
			$res = 0x10000 + $res;
		}
		printf("sub   (ACC)%04x = (ACC)%04x - [%03x]%04x\n", $res, $ACC, $address, $val);
		$ACC = $res;
	# and
	} elsif ($opcode == 0x7) {
		my $val = $MEM[$address];
		my $res = $ACC & $val;
		printf("and   (ACC)%04x = (ACC)%04x & [%03x]%04x\n", $res, $ACC, $address, $val);
		$ACC = $res;
	# or
	} elsif ($opcode == 0x8) {
		my $val = $MEM[$address];
		my $res = $ACC | $val;
		printf("or    (ACC)%04x = (ACC)%04x | [%03x]%04x\n", $res, $ACC, $address, $val);
		$ACC = $res;
	# xor
	} elsif ($opcode == 0x9) {
		my $val = $MEM[$address];
		my $res = $ACC ^ $val;
		printf("xor   (ACC)%04x = (ACC)%04x ^ [%03x]%04x\n", $res, $ACC, $address, $val);
		$ACC = $res;
	# not
	} elsif ($opcode == 0xA) {
		my $val = $MEM[$address];
		my $res = ~$ACC & 0xffff;
		printf("not   (ACC)%04x = ! (ACC)%04x\n", $res, $ACC);
		$ACC = $res;
	# nop
	} elsif ($opcode == 0xB) {
		print "nop\n";
	# jmp
	} elsif ($opcode == 0xC) {
		printf("jmp   [%03x]\n", $address);
		$PC = $address - 1
	# jmpe
	} elsif ($opcode == 0xD) {
		if ($ACC == 0) {
			printf("jmpe  [%03x]\n", $address);
			$PC = $address - 1;
		} else {
			printf("jmpe  (ACC)%04x\n", $ACC);
		}
	# jmpl
	} elsif ($opcode == 0xE) {
		if ($ACC >> 15 == 1) {
			printf("jmpl  [%03x]\n", $address);
			$PC = $address - 1;
		} else {
			printf("jmpl  (ACC)%04x\n", $ACC);
		}
	# brl
	} elsif ($opcode == 0xF) {
		$ACC = $PC + 1;
		$PC = $address - 1;
		printf("brl   [%03x]  (ACC)%04x\n", $address, $ACC);
	} else {
		print STDERR sprintf("Error: Unknown opcode '%04x'\n", $opcode);
		exit 1;
	}
	$PC = $PC + 1;
	if ($PC > 0xfff) {
		print STDERR sprintf("Error: Memory overflow (PC = 0x%04x)\n", $PC);
		exit 1;
	}
}
