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
    let callbacks = Callbacks { on_item, on_error };

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

unsafe extern "C" fn on_item(item: *const tmidl_sys::ApiItem, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);

    let meta = ApiItemMeta {
        name: CStr::from_ptr((*item).name).to_str().unwrap().to_owned(),
        doc: CStr::from_ptr((*item).doc).to_str().unwrap().to_owned(),
    };

    let item = match (*item).ty_ {
        tmidl_sys::ApiItemType::Opaque => ApiItem::Opaque { meta },
        tmidl_sys::ApiItemType::Interface => ApiItem::Interface {
            meta,
            functions: Vec::new(),
        },
    };

    context.api_file.items.push(item);
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
    items: Vec<ApiItem>,
}

#[derive(Serialize)]
#[serde(tag = "type")]
enum ApiItem {
    Opaque {
        #[serde(flatten)]
        meta: ApiItemMeta,
    },
    Interface {
        #[serde(flatten)]
        meta: ApiItemMeta,
        functions: Vec<String>,
    },
}

#[derive(Serialize)]
struct ApiItemMeta {
    name: String,
    doc: String,
}
