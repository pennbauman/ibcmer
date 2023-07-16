// IBCMer - Rust
//   Author:
//     Penn Bauman (pennbauman@protonmail.com)
use std::env;
use ibcmer::IttyBittyComputingMachine;


fn main() -> Result<(), ibcmer::Error> {
    let args: Vec<String> = env::args().collect();
    if args.len() == 1 {
        println!("Missing code file");
        return Ok(());
    }
    let mut machine = IttyBittyComputingMachine::from_file(&args[1])?;
    machine.run()?;

    Ok(())
}
