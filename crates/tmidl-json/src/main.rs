use std::{
    ffi::{c_void, CStr, CString},
    os::raw::c_char,
};

use codespan_reporting::{
    diagnostic::{Diagnostic, Label},
    files::SimpleFiles,
    term::{
        self,
        termcolor::{ColorChoice, StandardStream},
    },
};
use serde::Serialize;

use tmidl_sys::{parse_tmidl, Callbacks};

fn main() {
    let input_path = std::env::args().nth(1).unwrap();
    let input_str = std::fs::read_to_string(input_path).unwrap();
    let input = CString::new(input_str.clone()).unwrap();

    let mut context = Context::default();
    let callbacks = Callbacks {
        on_declaration: on_item,
        on_error,
    };

    let result =
        unsafe { parse_tmidl(input.as_ptr(), &callbacks, &mut context as *mut _ as *mut _) };

    if !result {
        let writer = StandardStream::stderr(ColorChoice::Always);
        let config = codespan_reporting::term::Config::default();
        let mut files = SimpleFiles::new();
        let file_id = files.add("input.h", input_str);

        for (message, position) in context.errors {
            let diagnostic = Diagnostic::error()
                .with_message(message.clone())
                .with_labels(vec![
                    Label::primary(file_id, position..(position + 1)).with_message(message)
                ]);
            term::emit(&mut writer.lock(), &config, &files, &diagnostic).unwrap();
        }
        return;
    }

    let json = serde_json::to_string_pretty(&context.api_file).unwrap();
    println!("{}", json);
}

unsafe fn c_to_string(c_str: *const c_char) -> String {
    CStr::from_ptr(c_str).to_str().unwrap().to_owned()
}

unsafe extern "C" fn on_item(
    declaration: *const tmidl_sys::ApiDeclaration,
    user_context: *mut c_void,
) {
    let context = &mut *(user_context as *mut Context);

    let meta = ApiDeclarationMeta {
        name: c_to_string((*declaration).name),
        doc: c_to_string((*declaration).doc),
    };

    let mut functions = Vec::new();
    for i in 0..(*declaration).function_count {
        let fnptr = *(*declaration).functions.offset(i as isize);
        functions.push(c_to_string((*fnptr).name));
    }

    let item = match (*declaration).ty_ {
        tmidl_sys::ApiDeclarationType::Opaque => ApiDeclaration::Opaque { meta },
        tmidl_sys::ApiDeclarationType::Interface => ApiDeclaration::Interface { meta, functions },
    };

    context.api_file.declarations.push(item);
}

unsafe extern "C" fn on_error(message: *const c_char, position: i64, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);
    let message = CStr::from_ptr(message);

    context
        .errors
        .push((message.to_string_lossy().to_string(), position as usize));
}

#[derive(Default)]
struct Context {
    errors: Vec<(String, usize)>,
    api_file: ApiFile,
}

#[derive(Default, Serialize)]
struct ApiFile {
    declarations: Vec<ApiDeclaration>,
}

#[derive(Serialize)]
#[serde(tag = "type")]
enum ApiDeclaration {
    Opaque {
        #[serde(flatten)]
        meta: ApiDeclarationMeta,
    },
    Interface {
        #[serde(flatten)]
        meta: ApiDeclarationMeta,
        functions: Vec<String>,
    },
}

#[derive(Serialize)]
struct ApiDeclarationMeta {
    name: String,
    doc: String,
}
