fn main() {
    let dst = cmake::build("../../");

    println!("cargo:rustc-link-search=native={}/lib", dst.display());
    println!("cargo:rustc-link-lib=static=tmidl");
}
