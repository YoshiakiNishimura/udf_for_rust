use cxx::let_cxx_string;
#[cxx::bridge(namespace = "udf")]
mod ffi {
    unsafe extern "C++" {
        include!("udf/udf_types.h");
        include!("udf/udf_bridge.h");

        type udf_bridge;

        fn new_udf_bridge() -> UniquePtr<udf_bridge>;

        fn load(self: &udf_bridge, path: &CxxString);

        fn service_count(self: &udf_bridge) -> usize;

        fn get_service_at(self: &udf_bridge, idx: usize) -> UniquePtr<Service>;

        fn function_count(self: &udf_bridge, service_idx: usize) -> usize;

        fn get_function_at(
            self: &udf_bridge,
            service_idx: usize,
            func_idx: usize,
        ) -> UniquePtr<Function>;

        fn input_column_count(self: &udf_bridge, service_idx: usize, func_idx: usize) -> usize;

        fn get_input_column_at(
            self: &udf_bridge,
            service_idx: usize,
            func_idx: usize,
            col_idx: usize,
        ) -> UniquePtr<Column>;

        fn output_column_count(self: &udf_bridge, service_idx: usize, func_idx: usize) -> usize;

        fn get_output_column_at(
            self: &udf_bridge,
            service_idx: usize,
            func_idx: usize,
            col_idx: usize,
        ) -> UniquePtr<Column>;

        fn get_index(self: &Column) -> i32;
        fn get_name(self: &Column) -> &CxxString;
        fn get_type_kind(self: &Column) -> &CxxString;

        fn get_index(self: &Function) -> i32;
        fn get_name(self: &Function) -> &CxxString;
        fn get_kind(self: &Function) -> &CxxString;

        fn get_name(self: &Service) -> &CxxString;

        type Column;
        type Function;
        type Service;
    }
}

use serde::Serialize;

#[derive(Serialize)]
pub struct ColumnRust {
    pub index: i32,
    pub name: String,
    pub type_kind: String,
}

#[derive(Serialize)]
pub struct FunctionRust {
    pub index: i32,
    pub name: String,
    pub kind: String,
    pub input_columns: Vec<ColumnRust>,
    pub output_columns: Vec<ColumnRust>,
}

#[derive(Serialize)]
pub struct ServiceRust {
    pub name: String,
    pub functions: Vec<FunctionRust>,
}

pub fn dump_json(plugin_dir: &str) -> Result<String, Box<dyn std::error::Error>> {
    let bridge = ffi::new_udf_bridge();
    let_cxx_string!(dir = plugin_dir);
    bridge.load(&dir);
    let mut services = Vec::new();

    let svc_count = bridge.service_count();
    for i in 0..svc_count {
        let svc_ptr = bridge.get_service_at(i);
        if svc_ptr.is_null() {
            continue;
        }
        let svc = svc_ptr.as_ref().unwrap();
        let name = svc.get_name().to_string_lossy().into_owned();

        let func_count = bridge.function_count(i);
        let mut functions = Vec::new();

        for j in 0..func_count {
            let func_ptr = bridge.get_function_at(i, j);
            if func_ptr.is_null() {
                continue;
            }
            let func = func_ptr.as_ref().unwrap();
            let mut input_columns = Vec::new();
            for k in 0..bridge.input_column_count(i, j) {
                let col_ptr = bridge.get_input_column_at(i, j, k);
                if col_ptr.is_null() {
                    continue;
                }
                let col = col_ptr.as_ref().unwrap();
                input_columns.push(ColumnRust {
                    index: col.get_index(),
                    name: col.get_name().to_string_lossy().into_owned(),
                    type_kind: col.get_type_kind().to_string_lossy().into_owned(),
                });
            }

            let mut output_columns = Vec::new();
            for k in 0..bridge.output_column_count(i, j) {
                let col_ptr = bridge.get_output_column_at(i, j, k);
                if col_ptr.is_null() {
                    continue;
                }
                let col = col_ptr.as_ref().unwrap();
                output_columns.push(ColumnRust {
                    index: col.get_index(),
                    name: col.get_name().to_string_lossy().into_owned(),
                    type_kind: col.get_type_kind().to_string_lossy().into_owned(),
                });
            }

            functions.push(FunctionRust {
                index: func.get_index(),
                name: func.get_name().to_string_lossy().into_owned(),
                kind: func.get_kind().to_string_lossy().into_owned(),
                input_columns,
                output_columns,
            });
        }

        services.push(ServiceRust { name, functions });
    }

    let json = serde_json::to_string_pretty(&services)?;
    Ok(json)
}
