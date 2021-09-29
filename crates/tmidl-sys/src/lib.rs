use std::{ffi::c_void, os::raw::c_char};

#[repr(C)]
pub struct ApiDeclaration {
    pub ty_: ApiDeclarationType,
    pub name: *const c_char,
    pub doc: *const c_char,
    pub functions: *const *const ApiFunction,
    pub function_count: i32,
}

#[repr(C)]
pub enum ApiDeclarationType {
    Opaque,
    Interface,
}

#[repr(C)]
pub struct ApiFunction {
    pub name: *const c_char,
}

#[repr(C)]
pub struct Callbacks {
    pub on_declaration:
        unsafe extern "C" fn(declaration: *const ApiDeclaration, user_context: *mut c_void),
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
