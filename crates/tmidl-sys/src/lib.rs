use std::{ffi::c_void, os::raw::c_char};

#[repr(C)]
pub struct Callbacks {
    pub on_item: extern "C" fn(user_context: *mut c_void),
}

extern "C" {
    pub fn parse_tmidl(
        input: *const c_char,
        callbacks: *const Callbacks,
        user_context: *mut c_void,
    ) -> bool;
}
