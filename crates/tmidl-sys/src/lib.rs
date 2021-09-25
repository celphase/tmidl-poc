use std::{ffi::c_void, os::raw::c_char};

#[repr(C)]
pub struct Callbacks {
    pub on_item_opaque: unsafe extern "C" fn(name: *const c_char, user_context: *mut c_void),
    pub on_item_interface: unsafe extern "C" fn(name: *const c_char, user_context: *mut c_void),
}

extern "C" {
    pub fn parse_tmidl(
        input: *const c_char,
        callbacks: *const Callbacks,
        user_context: *mut c_void,
    ) -> bool;
}
