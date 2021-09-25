use std::{ffi::c_void, os::raw::c_char};

#[repr(C)]
pub struct Item {
    pub ty_: ItemType,
    pub name: *const c_char,
    pub doc: *const c_char,
}

#[repr(C)]
pub enum ItemType {
    Opaque,
    Interface,
}

#[repr(C)]
pub struct Callbacks {
    pub on_item: unsafe extern "C" fn(item: *const Item, user_context: *mut c_void),
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
