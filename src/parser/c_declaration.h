#pragma once

typedef enum c_storage_class_t {
    C_STORAGE_CLASS_NONE,
    C_STORAGE_CLASS_TYPEDEF,
} c_storage_class_t;

typedef struct c_declaration_t {
    c_storage_class_t storage_class;
} c_declaration_t;
