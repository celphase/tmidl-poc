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

## License

Licensed under either of

- Apache License, Version 2.0 ([LICENSE-APACHE](LICENSE-APACHE) or http://www.apache.org/licenses/LICENSE-2.0)
- MIT License (Expat) ([LICENSE-MIT](LICENSE-MIT) or http://opensource.org/licenses/MIT)

at your option.
