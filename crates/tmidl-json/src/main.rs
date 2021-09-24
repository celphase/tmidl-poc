use std::ffi::{c_void, CString};

use tmidl_sys::{parse_tmidl, Callbacks};

fn main() {
    let input_path = std::env::args().nth(1).unwrap();
    let input = std::fs::read_to_string(input_path).unwrap();
    let input = CString::new(input).unwrap();

    let mut user_context = UserContext {};
    let callbacks = Callbacks { next_codepoint };

    let result = unsafe {
        parse_tmidl(
            input.as_ptr(),
            &callbacks,
            &mut user_context as *mut _ as *mut _,
        )
    };

    if !result {
        println!("Parsing failed");
    }
}

struct UserContext {}

extern "C" fn next_codepoint(user_context: *mut c_void) -> i32 {
    let _user_context: &mut UserContext = unsafe { &mut *(user_context as *mut UserContext) };
    -1
}
