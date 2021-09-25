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

    let mut context = Context::default();
    let callbacks = Callbacks {
        on_item_opaque,
        on_item_interface,
        on_error,
    };

    let result =
        unsafe { parse_tmidl(input.as_ptr(), &callbacks, &mut context as *mut _ as *mut _) };

    if !result {
        for error in context.errors {
            println!("{}", error);
        }
        return;
    }

    let json = serde_json::to_string_pretty(&context.api_file).unwrap();
    println!("{}", json);
}

unsafe extern "C" fn on_item_opaque(name: *const c_char, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);
    let name = CStr::from_ptr(name);

    context
        .api_file
        .opaques
        .push(name.to_string_lossy().to_string());
}

unsafe extern "C" fn on_item_interface(name: *const c_char, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);
    let name = CStr::from_ptr(name);

    context
        .api_file
        .interfaces
        .push(name.to_string_lossy().to_string());
}

unsafe extern "C" fn on_error(message: *const c_char, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);
    let message = CStr::from_ptr(message);

    context.errors.push(message.to_string_lossy().to_string());
}

#[derive(Default)]
struct Context {
    errors: Vec<String>,
    api_file: ApiFile,
}

#[derive(Default, Serialize)]
struct ApiFile {
    opaques: Vec<String>,
    interfaces: Vec<String>,
}
