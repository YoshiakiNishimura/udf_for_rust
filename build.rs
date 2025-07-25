fn main() {
    cxx_build::bridge("src/lib.rs")
        .file("cpp/udf_bridge.cpp")
        .file("cpp/udf_loader.cpp")
        .file("cpp/udf_types.cpp")
        .include("include")
        .flag_if_supported("-std=c++17")
        .compile("tsurugi_udf");

    println!("cargo:rerun-if-changed=src/lib.rs");
    println!("cargo:rerun-if-changed=cpp/udf_bridge.cpp");
    println!("cargo:rerun-if-changed=cpp/udf_loader.cpp");
    println!("cargo:rerun-if-changed=include/udf/udf_bridge.h");
    println!("cargo:rerun-if-changed=include/udf/udf_loader.h");
    println!("cargo:rerun-if-changed=include/udf/udf_types.h");
}

