mod wrapper;

use codespan_reporting::{
    diagnostic::{Diagnostic, Label, Severity},
    files::SimpleFiles,
    term::{
        self,
        termcolor::{ColorChoice, StandardStream},
    },
};

use crate::wrapper::{parse_tmidl, Diagnostic as TmidlDiagnostic};

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

fn print_diagnostics(diagnostics: Vec<TmidlDiagnostic>, input_str: String) {
    let writer = StandardStream::stderr(ColorChoice::Always);
    let config = codespan_reporting::term::Config::default();
    let mut files = SimpleFiles::new();
    let file_id = files.add("input.h", input_str);

    for diagnostic in diagnostics {
        let severity = if diagnostic.is_error {
            Severity::Error
        } else {
            Severity::Warning
        };

        let diagnostic = Diagnostic::new(severity)
            .with_message(diagnostic.message.clone())
            .with_labels(vec![Label::primary(
                file_id,
                diagnostic.position_start..diagnostic.position_end,
            )
            .with_message(diagnostic.message)]);
        term::emit(&mut writer.lock(), &config, &files, &diagnostic).unwrap();
    }
}
