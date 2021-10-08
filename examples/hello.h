#pragma once

// An example opaque type.
typedef struct hello_o hello_o;

// An example interface.
// Multi-line comments are supported too!
typedef struct hello_i
{
    // Creates a hello instance.
    hello_o* (*create)();

    // Destroys a hello instance.
    void (*destroy)(hello_o *inst);
} hello_i;

typedef struct uncommented_o uncommented_o;
