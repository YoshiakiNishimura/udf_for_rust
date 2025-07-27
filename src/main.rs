use std::env;
use std::error::Error;

fn main() -> Result<(), Box<dyn Error>> {
    let args: Vec<String> = env::args().collect();

    if args.len() < 2 {
        eprintln!("Usage: {} <plugin_path>", args[0]);
        std::process::exit(1);
    }

    let plugin_path = &args[1];

    let json = tsurugi_dump_udf::dump_json(plugin_path)?;
    println!("{}", json);

    Ok(())
}
