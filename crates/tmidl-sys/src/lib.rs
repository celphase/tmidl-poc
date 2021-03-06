use std::{ffi::c_void, os::raw::c_char};

#[repr(C)]
pub struct ParserO {
    private: [u8; 0],
}

#[repr(C)]
pub struct Declaration {
    pub ty_: DeclarationType,
    pub name: *const c_char,
    pub doc: *const c_char,
    pub functions: *const *const Function,
    pub function_count: i32,
}

#[repr(C)]
pub enum DeclarationType {
    Opaque,
    Interface,
}

#[repr(C)]
pub struct Function {
    pub name: *const c_char,
}

#[repr(C)]
pub struct Diagnostic {
    pub level: Level,
    pub message: *const c_char,
    pub position_start: i32,
    pub position_end: i32,
}

#[repr(C)]
#[derive(PartialEq, Eq)]
pub enum Level {
    Warning,
    Error,
}

#[repr(C)]
pub struct Callbacks {
    pub on_declaration:
        unsafe extern "C" fn(declaration: *const Declaration, user_context: *mut c_void),
    pub on_module_doc: unsafe extern "C" fn(doc: *const c_char, user_context: *mut c_void),
    pub on_diagnostic:
        unsafe extern "C" fn(diagnostic: *const Diagnostic, user_context: *mut c_void),
}

extern "C" {
    pub fn tmidl_parser_create() -> *mut ParserO;

    pub fn tmidl_parser_destroy(parser: *mut ParserO);

    pub fn tmidl_parser_parse(
        parser: *mut ParserO,
        input: *const c_char,
        callbacks: *const Callbacks,
        user_context: *mut c_void,
    ) -> bool;
}
