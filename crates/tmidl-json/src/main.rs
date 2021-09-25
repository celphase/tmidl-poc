use std::{
    ffi::{c_void, CStr, CString},
    os::raw::c_char,
};

use serde::Serialize;

use tmidl_sys::{parse_tmidl, Callbacks};

fn main() {
    let input_path = std::env::args().nth(1).unwrap();
    let input = std::fs::read_to_string(input_path).unwrap();
    let input = CString::new(input).unwrap();

    let mut descriptor = ApiFile::default();
    let callbacks = Callbacks {
        on_item_opaque,
        on_item_interface,
    };

    let result = unsafe {
        parse_tmidl(
            input.as_ptr(),
            &callbacks,
            &mut descriptor as *mut _ as *mut _,
        )
    };

    if !result {
        println!("Parsing failed");
        return;
    }

    let json = serde_json::to_string_pretty(&descriptor).unwrap();
    println!("{}", json);
}

#[derive(Default, Serialize)]
struct ApiFile {
    opaques: Vec<String>,
    interfaces: Vec<String>,
}

unsafe extern "C" fn on_item_opaque(name: *const c_char, user_context: *mut c_void) {
    let api_file = &mut *(user_context as *mut ApiFile);
    let name = CStr::from_ptr(name);

    api_file
        .opaques
        .push(name.to_string_lossy().to_string());
}

unsafe extern "C" fn on_item_interface(name: *const c_char, user_context: *mut c_void) {
    let api_file = &mut *(user_context as *mut ApiFile);
    let name = CStr::from_ptr(name);

    api_file
        .interfaces
        .push(name.to_string_lossy().to_string());
}
