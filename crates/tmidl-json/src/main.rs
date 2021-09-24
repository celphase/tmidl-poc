use tmidl_sys::parse_tmidl;

fn main() {
    let result = unsafe { parse_tmidl() };

    if !result {
        println!("Parsing failed");
    }
}
