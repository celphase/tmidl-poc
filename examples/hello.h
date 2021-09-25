#pragma once

// An example opaque type.
typedef struct hello_o hello_o;

// An example interface.
// Multi-line comments are supported too!
typedef struct hello_i
{
    // Creates an hello instance.
    hello_o* (*create)();

    // Destroys an hello instance.
    void (*destroy)(hello_o *inst);
} hello_i;
