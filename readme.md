# The Machinery IDL - Proof of Concept

This is a proof of concept for an Interface Description Language isomorphic with C for describing
C11 ABI compatible APIs for The Machinery game engine.

The TMIDL parser is based on the [Micro Parser Combinators](https://github.com/orangeduck/mpc)
library, vendored in `vendor/mpc`.

## Motivation

It is desirable to describe APIs in a machine-readable format for generating bindings between
languages, including necessary metadata beyond just the C ABI alone to generate these bindings
correctly.
However, writing describing these APIs in a custom DSL can be cumbersome and unclear, especially
with large existing codebases.

TMIDL is a interface description language that compiles as standard C11 headers (isomorphic), while
using naming convention, preprocessor directives, and macros to provide additional information.

It can be used to generate bindings for other languages, and to generate documentation.
Additionally it is also a linter to ensure APIs conform to The Machinery code conventions.

# Isomorphism

TMIDL is isomorphic with C11, meaning all valid TMIDL can be compiled as C code for the purpose of
using a described API.
However, the inverse is not true, not all valid C11 code is valid TMIDL.

In addition to some language features not being supported, TMIDL treats preprocessor directives and
macros as relevant metadata, and will for this reason parse them syntactically at the same time as
the rest of the code.
These are used as metadata information necessary to generate correct bindings.
However, this means not all C preprocessor tricks can be supported.

# Examples

## tmidl-read

This example reads in a TMIDL file and writes the file's data to console in human-readable form.
This is not intended to be machine-readable, it's example API usage.

## tmidl-json

This example reads in a TMIDL file and writes the file's data to console in JSON format.
This is an example of using the TMIDL API from other languages.

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
- MIT License (Expat) ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.
