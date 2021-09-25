#pragma once

// An example opaque type.
typedef struct hello_o hello_o;

// An example interface.
typedef struct hello_i
{
    hello_o* (*create)();

    void (*destroy)(hello_o *inst);
} hello_i;
