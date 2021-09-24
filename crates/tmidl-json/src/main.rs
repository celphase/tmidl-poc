use std::{ffi::c_void, str::Chars};

use tmidl_sys::{parse_tmidl, Callbacks};

fn main() {
    let input_path = std::env::args().nth(1).unwrap();
    let input = std::fs::read_to_string(input_path).unwrap();

    let mut context = Context {
        input: input.chars(),
    };
    let callbacks = Callbacks { next_codepoint };

    let result = unsafe { parse_tmidl(&callbacks, &mut context as *mut _ as *mut _) };

    if !result {
        println!("Parsing failed");
    }
}

struct Context<'a> {
    input: Chars<'a>,
}

extern "C" fn next_codepoint(context: *mut c_void) -> i32 {
    let context: &mut Context = unsafe { &mut *(context as *mut Context) };
    context.input.next().map(|c| c as i32).unwrap_or(-1)
}
