use std::ffi::c_void;

#[repr(C)]
pub struct Callbacks {
    pub next_codepoint: extern "C" fn(context: *mut c_void) -> i32,
}

extern "C" {
    pub fn parse_tmidl(callbacks: *const Callbacks, context: *mut c_void) -> bool;
}
