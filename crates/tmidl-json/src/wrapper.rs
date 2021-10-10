use std::{
    ffi::{c_void, CStr, CString},
    os::raw::c_char,
};

use serde::Serialize;

pub fn parse_tmidl(input: &str) -> (Option<Module>, Vec<Diagnostic>) {
    let input = CString::new(input).unwrap();

    let mut context = Context::default();
    let callbacks = tmidl_sys::Callbacks {
        on_declaration,
        on_module_doc,
        on_diagnostic,
    };

    let result = unsafe {
        let parser = tmidl_sys::tmidl_parser_create();
        let result = tmidl_sys::tmidl_parser_parse(
            parser,
            input.as_ptr(),
            &callbacks,
            &mut context as *mut _ as *mut _,
        );
        tmidl_sys::tmidl_parser_destroy(parser);
        result
    };

    let value = if result { Some(context.module) } else { None };

    (value, context.diagnostics)
}

unsafe fn c_to_string(c_str: *const c_char) -> String {
    CStr::from_ptr(c_str).to_str().unwrap().to_owned()
}

unsafe extern "C" fn on_declaration(
    declaration: *const tmidl_sys::Declaration,
    user_context: *mut c_void,
) {
    let context = &mut *(user_context as *mut Context);

    let meta = DeclarationMeta {
        name: c_to_string((*declaration).name),
        doc: c_to_string((*declaration).doc),
    };

    let mut functions = Vec::new();
    for i in 0..(*declaration).function_count {
        let fnptr = *(*declaration).functions.offset(i as isize);
        functions.push(c_to_string((*fnptr).name));
    }

    let item = match (*declaration).ty_ {
        tmidl_sys::DeclarationType::Opaque => Declaration::Opaque { meta },
        tmidl_sys::DeclarationType::Interface => Declaration::Interface { meta, functions },
    };

    context.module.declarations.push(item);
}

unsafe extern "C" fn on_module_doc(doc: *const c_char, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);
    let doc = CStr::from_ptr(doc);

    context.module.doc.push_str(&doc.to_string_lossy());
}

unsafe extern "C" fn on_diagnostic(
    diagnostic: *const tmidl_sys::Diagnostic,
    user_context: *mut c_void,
) {
    let context = &mut *(user_context as *mut Context);
    let message = CStr::from_ptr((*diagnostic).message);

    let diagnostic = Diagnostic {
        is_error: (*diagnostic).level == tmidl_sys::Level::Error,
        message: message.to_string_lossy().to_string(),
        position_start: (*diagnostic).position_start as usize,
        position_end: (*diagnostic).position_end as usize,
    };

    context.diagnostics.push(diagnostic);
}

#[derive(Default)]
struct Context {
    diagnostics: Vec<Diagnostic>,
    module: Module,
}

#[derive(Default, Serialize)]
pub struct Module {
    doc: String,
    declarations: Vec<Declaration>,
}

#[derive(Serialize)]
#[serde(tag = "type")]
enum Declaration {
    Opaque {
        #[serde(flatten)]
        meta: DeclarationMeta,
    },
    Interface {
        #[serde(flatten)]
        meta: DeclarationMeta,
        functions: Vec<String>,
    },
}

#[derive(Serialize)]
struct DeclarationMeta {
    name: String,
    doc: String,
}

pub struct Diagnostic {
    pub is_error: bool,
    pub message: String,
    pub position_start: usize,
    pub position_end: usize,
}
