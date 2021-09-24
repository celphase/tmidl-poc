#pragma once

typedef struct hello_o hello_o;

typedef struct hello_i
{
    hello_o* (*create)();

    void (*destroy)(hello_o *inst);
};
