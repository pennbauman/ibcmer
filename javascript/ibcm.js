// IBCMer - JavaScript
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
export const MEM_SIZE = 4096;

export class IBCM {
	constructor() {
		this.acc = 0;
		this.pc = 0;
		this.mem = Array(MEM_SIZE).fill(0);
		this.halted = false;
	}

	// Load from string
	async fromString(txt) {
		this.acc = 0;
		this.pc = 0;
		this.mem = Array(MEM_SIZE).fill(0);
		this.halted = false;
		if (txt.trim() === "") {
			return;
		}
		let lines = txt.trim().split("\n");
		for (let i in lines) {
			let opcode = lines[i].substring(0, 4).trim();
			if (opcode.length != 4) {
				throw `Invalid hex '${opcode}' on line ${i}: too short`;
			}
			try {
				this.mem[i] = parseInt(Number("0x" + opcode), 10);
			} catch (e) {
				throw `Invalid hex '${opcode}' on line ${i}`;
			}
			i++;
		}
	}

	// Export memory as string
	async toString() {
		var last = MEM_SIZE - 1;
		while (this.mem[last] === 0) {
			last--;
			if (last === 0) {
				break;
			}
		}
		var ret = "";
		for (let i = 0; i <= last; i++) {
			ret += this.mem[i].toString(16).padStart(4, 0) + "\n";
		}
		return ret;
	}

	// Print machine state
	async print() {
		console.log("IBCMer");
		console.log("ACC:  ", this.acc.toString(16).padStart(4, 0));
		console.log("PC:   ", this.pc.toString(16).padStart(3, 0));
		var last = MEM_SIZE - 1;
		while (this.mem[last] === 0) {
			last--;
			if (last === 0) {
				break;
			}
		}
		last += 4;
		for (let i = 0; i < last; i++) {
			console.log("[" + i.toString(16).padStart(3, 0) + "]:", this.mem[i].toString(16).padStart(4, 0));
		}
	}

	// Execute current operation and increment PC
	async step(input, output, log) {
		if (this.halted === true) {
			throw "IBCM halted";
		}
		let opcode = this.mem[this.pc] & 0xFFFF;
		let addr = opcode & 0xFFF;
		let log_prefix = "[" + this.pc.toString(16).padStart(3, 0) + "]" + opcode.toString(16).padStart(4, 0) + "  ";
		switch (opcode >> 12) {
			case 0x0: // halt
				this.halted = true;
				log(log_prefix + "halt");
				return;
			case 0x1: // i/o
				log(log_prefix + "i/o   (ACC)" + this.acc.toString(16).padStart(4, 0));
				let message = null;
				switch(addr >> 8) {
					case 0x0:
						while (true) {
							let text = await input("Input hex:  ", message);
							if (text.trim().length > 4) {
								message = "Enter 4 digit hexadecimal";
								continue;
							}
							try {
								this.acc = parseInt(Number("0x" + text.trim()), 10);
								break;
							} catch(e) {
								message = "Enter valid hexadecimal";
							}
						}
						break;
					case 0x4:
						let i = 0;
						while (true) {
							let text = await input("Input char: ", message);
							// console.log("i/o char:", text);
							if (text.length === 1) {
								this.acc = text.charCodeAt(0);
								break;
							} else {
								message = "Enter single character";
							}
							i++;
							if (i === 10) {
								process.exit();
							}
						}
						break;
					case 0x8:
						output("Output hex:  " + this.acc.toString(16).padStart(4, 0));
						break;
					case 0xC:
						output("Output char: " + String.fromCharCode(this.acc));
						break;
					default:
						throw "Invalid i/o subopcode " + (addr >> 8);
				}
				break;
			case 0x2: // shift
                let old = this.acc.toString(16).padStart(4, 0);
                let distance = opcode & 0xf;
				switch(addr >> 8) {
					case 0x0:
                        this.acc = (this.acc << distance) & 0xffff;
                        log(log_prefix + "shift (ACC)" + this.acc.toString(16).padStart(4, 0) + " = (ACC)" + old + " << " + distance.toString(16));
						break;
					case 0x4:
                        this.acc = this.acc >> distance;
                        log(log_prefix + "shift (ACC)" + this.acc.toString(16).padStart(4, 0) + " = (ACC)" + old + " >> " + distance.toString(16));
						break;
					case 0x8:
						this.acc = ((this.acc << distance) | (this.acc >> (16 - distance))) & 0xffff
                        log(log_prefix + "shift (ACC)" + this.acc.toString(16).padStart(4, 0) + " = (ACC)" + old + " <= " + distance.toString(16));
						break;
					case 0xC:
						this.acc = ((this.acc >> distance) | (this.acc << (16 - distance))) & 0xffff
                        log(log_prefix + "shift (ACC)" + this.acc.toString(16).padStart(4, 0) + " = (ACC)" + old + " => " + distance.toString(16));
						break;
					default:
						throw "Invalid shift subopcode " + (addr >> 8);
				}
				break;
			case 0x3: // load
				this.acc = this.mem[addr];
				var acc = this.acc.toString(16).padStart(4, 0);
				log(log_prefix + "load  (ACC)" + acc + " = [" + addr.toString(16).padStart(3, 0) + "]" + acc);
				break;
			case 0x4: // store
				var acc = this.acc.toString(16).padStart(4, 0);
				log(log_prefix + "store [" + addr.toString(16).padStart(3, 0) + "]" + acc + " = (ACC)" + acc);
				this.mem[addr] = this.acc;
				break;
			case 0x5: // add
				var res = (this.acc + this.mem[addr]) & 0xFFFF;
				log(log_prefix + "add   (ACC)" + res.toString(16).padStart(4, 0) + " = (ACC)" + this.acc.toString(16).padStart(4, 0) + " + [" + addr.toString(16).padStart(3, 0) + "]" + this.mem[addr].toString(16).padStart(4, 0));
				this.acc = res;
				break;
			case 0x6: // sub
				var res = this.acc - this.mem[addr];
				if (res < 0) {
					res = res + 0x10000;
				}
				log(log_prefix + "sub   (ACC)" + res.toString(16).padStart(4, 0) + " = (ACC)" + this.acc.toString(16).padStart(4, 0) + " - [" + addr.toString(16).padStart(3, 0) + "]" + this.mem[addr].toString(16).padStart(4, 0));
				this.acc = res;
				break;
			case 0x7: // and
				var res = this.acc & this.mem[addr];
				log(log_prefix + "and   (ACC)" + res.toString(16).padStart(4, 0) + " = (ACC)" + this.acc.toString(16).padStart(4, 0) + " & [" + addr.toString(16).padStart(3, 0) + "]" + this.mem[addr].toString(16).padStart(4, 0));
				this.acc = res;
				break;
			case 0x8: // or
				var res = this.acc | this.mem[addr];
				log(log_prefix + "or    (ACC)" + res.toString(16).padStart(4, 0) + " = (ACC)" + this.acc.toString(16).padStart(4, 0) + " | [" + addr.toString(16).padStart(3, 0) + "]" + this.mem[addr].toString(16).padStart(4, 0));
				this.acc = res;
				break;
			case 0x9: // xor
				var res = this.acc ^ this.mem[addr];
				log(log_prefix + "xor   (ACC)" + res.toString(16).padStart(4, 0) + " = (ACC)" + this.acc.toString(16).padStart(4, 0) + " ^ [" + addr.toString(16).padStart(3, 0) + "]" + this.mem[addr].toString(16).padStart(4, 0));
				this.acc = res;
				break;
			case 0xA: // not
				var res = this.acc ^ 0xffff;
                log(log_prefix + "not   (ACC)" + res.toString(16).padStart(4, 0) + " = ! (ACC)" + this.acc.toString(16).padStart(4, 0));
				this.acc = res;
				break;
			case 0xB: // nop
				log(log_prefix + "nop");
				break;
			case 0xC: // jmp
				log(log_prefix + "jmp   [" + addr.toString(16).padStart(3, 0) + "]");
				this.pc = addr;
				return;
			case 0xD: // jmpe
				if (this.acc === 0) {
					log(log_prefix + "jmpe  [" + addr.toString(16).padStart(3, 0) + "]");
					this.pc = addr;
					return;
				}
				log(log_prefix + "jmpe  (ACC)" + this.acc.toString(16).padStart(4, 0));
				break;
			case 0xE: // jmpl
				if (this.acc >> 15 === 1) {
					log(log_prefix + "jmpl  [" + addr.toString(16).padStart(3, 0) + "]");
					this.pc = addr;
					return;
				}
				log(log_prefix + "jmpl  (ACC)" + this.acc.toString(16).padStart(4, 0));
				break;
			case 0xF: // brl
				this.acc = this.pc + 1;
				this.pc = addr;
				log(log_prefix + "brl   [" + addr.toString(16).padStart(3, 0) + "]  (ACC)" + this.acc.toString(16).padStart(4, 0));
				return;
			default:
				throw "Impossible opcode " + opcode;
		}
		this.pc++;
		if (this.pc >= MEM_SIZE) {
			throw "Memory overflow (PC = 0x" + this.pc.toString(16).padStart(4, 0) + ")";
		}
	}
}
