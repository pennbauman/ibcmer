// IBCMer - Rust
//   URL: https://github.com/pennbauman/ibcmer
//   License: Creative Commons Attribution Share Alike 4.0 International
//   Author: Penn Bauman <me@pennbauman.com>
use std::env;
use ibcmer::IttyBittyComputingMachine;


fn main() -> Result<(), ibcmer::Error> {
    let args: Vec<String> = env::args().collect();
    if args.len() == 1 {
        println!("Missing code file");
        return Ok(());
    }
    match IttyBittyComputingMachine::from_file(&args[1]) {
        Ok(mut machine) => match machine.run() {
            Ok(()) => (),
            Err(e) => {
                eprintln!("Error: {}", e);
                std::process::exit(1);
            },
        },
        Err(e) => {
            eprintln!("Error: {}", e);
            std::process::exit(1);
        },
    };

    Ok(())
}
