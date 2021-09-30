mod wrapper;

use codespan_reporting::{
    diagnostic::{Diagnostic, Label},
    files::SimpleFiles,
    term::{
        self,
        termcolor::{ColorChoice, StandardStream},
    },
};

use crate::wrapper::parse_tmidl;

fn main() {
    let input_path = std::env::args().nth(1).unwrap();
    let input = std::fs::read_to_string(input_path).unwrap();

    let (result, diagnostics) = parse_tmidl(&input);

    // Print messages if there are any
    if !diagnostics.is_empty() {
        print_diagnostics(diagnostics, input);
        std::process::exit(1);
    }

    let api_file = if let Some(api_file) = result {
        api_file
    } else {
        std::process::exit(1);
    };

    let json = serde_json::to_string_pretty(&api_file).unwrap();
    println!("{}", json);
}

fn print_diagnostics(diagnostics: Vec<(String, usize)>, input_str: String) {
    let writer = StandardStream::stderr(ColorChoice::Always);
    let config = codespan_reporting::term::Config::default();
    let mut files = SimpleFiles::new();
    let file_id = files.add("input.h", input_str);

    for (message, position) in diagnostics {
        let diagnostic = Diagnostic::error()
            .with_message(message.clone())
            .with_labels(vec![
                Label::primary(file_id, position..(position + 1)).with_message(message)
            ]);
        term::emit(&mut writer.lock(), &config, &files, &diagnostic).unwrap();
    }
}
