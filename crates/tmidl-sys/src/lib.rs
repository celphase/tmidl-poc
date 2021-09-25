use std::{ffi::c_void, os::raw::c_char};

#[repr(C)]
pub struct ApiItem {
    pub ty_: ApiItemType,
    pub name: *const c_char,
    pub doc: *const c_char,
}

#[repr(C)]
pub enum ApiItemType {
    Opaque,
    Interface,
}

#[repr(C)]
pub struct Callbacks {
    pub on_item: unsafe extern "C" fn(item: *const ApiItem, user_context: *mut c_void),
    pub on_error:
        unsafe extern "C" fn(message: *const c_char, position: i64, user_context: *mut c_void),
}

extern "C" {
    pub fn parse_tmidl(
        input: *const c_char,
        callbacks: *const Callbacks,
        user_context: *mut c_void,
    ) -> bool;
}
