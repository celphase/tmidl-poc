use std::{
    ffi::{c_void, CStr, CString},
    os::raw::c_char,
};

use tmidl_sys::{parse_tmidl, Callbacks};

fn main() {
    let input_path = std::env::args().nth(1).unwrap();
    let input = std::fs::read_to_string(input_path).unwrap();
    let input = CString::new(input).unwrap();

    let mut user_context = UserContext::default();
    let callbacks = Callbacks {
        on_item_opaque,
        on_item_interface,
    };

    let result = unsafe {
        parse_tmidl(
            input.as_ptr(),
            &callbacks,
            &mut user_context as *mut _ as *mut _,
        )
    };

    if !result {
        println!("Parsing failed");
        return;
    }

    for opaque in user_context.opaques {
        println!("Opaque: {}", opaque);
    }

    for interface in user_context.interfaces {
        println!("Interface: {}", interface);
    }
}

#[derive(Default)]
struct UserContext {
    opaques: Vec<String>,
    interfaces: Vec<String>,
}

unsafe extern "C" fn on_item_opaque(name: *const c_char, user_context: *mut c_void) {
    let user_context = &mut *(user_context as *mut UserContext);
    let name = CStr::from_ptr(name);

    user_context
        .opaques
        .push(name.to_string_lossy().to_string());
}

unsafe extern "C" fn on_item_interface(name: *const c_char, user_context: *mut c_void) {
    let user_context = &mut *(user_context as *mut UserContext);
    let name = CStr::from_ptr(name);

    user_context
        .interfaces
        .push(name.to_string_lossy().to_string());
}
