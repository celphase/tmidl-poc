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
        on_item_opaque,
        on_item_interface,
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

unsafe extern "C" fn on_item_opaque(name: *const c_char, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);
    let name = CStr::from_ptr(name);

    let item = Item {
        ty_: ItemType::Opaque,
        name: name.to_string_lossy().to_string(),
    };
    context.api_file.items.push(item);
}

unsafe extern "C" fn on_item_interface(name: *const c_char, user_context: *mut c_void) {
    let context = &mut *(user_context as *mut Context);
    let name = CStr::from_ptr(name);

    let item = Item {
        ty_: ItemType::Interface,
        name: name.to_string_lossy().to_string(),
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
    items: Vec<Item>,
}

#[derive(Serialize)]
struct Item {
    #[serde(rename = "type")] 
    ty_: ItemType,
    name: String,
}

#[derive(Serialize)]
enum ItemType {
    Opaque,
    Interface,
}
