#!/usr/bin/env node
// IBCMer - Node.js
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
import * as fs from 'fs';
import { Channel } from 'async-channel';

import { IBCM } from './ibcm.js';


// Get code file
if (process.argv.length === 2) {
	console.error("Missing code file arguments");
	process.exit(1);
}
var machine = new IBCM();

// Setup IBCM machine
try {
	const data = fs.readFileSync(process.argv[2], 'utf8');
	await machine.fromString(data, process.argv[2]);
} catch (err) {
	console.error("Error:", err);
	process.exit(1);
}
// machine.print();

// Liston on stdin and place lines in channel
const chan = new Channel(256);
process.stdin.resume();
process.stdin.on('data', (d) => {
	d.toString().trim().split("\n").forEach((l) => chan.push(l));
});

// Ask for input and get stdin lines from channel
async function inputBuffered(query, message) {
	if (message) {
		console.error("Error:", message);
		process.exit(1);
	}
	process.stdout.write(query);
	return await chan;
}

// Run IBCM machine
while (!machine.halted) {
	try {
		await machine.step(inputBuffered, console.log, console.log);
	} catch (e) {
		console.error("Error:", e);
		process.exit(1)
	}
}

// Close stdin
process.stdin.pause();
