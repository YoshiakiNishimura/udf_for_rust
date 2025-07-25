fn main() -> Result<(), Box<dyn std::error::Error>> {
    let plugin_dir = "/home/nishimura/plugin";

    let json = tsurugi_dump_udf::dump_json(plugin_dir)?;

    println!("{}", json);

    Ok(())
}
