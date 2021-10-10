#include <malloc.h>
#include <stdio.h>
#include <tmidl.h>

static void on_declaration(const tmidl_declaration_t *declaration, void *user_context)
{
    printf("# %s\n", declaration->name);

    switch (declaration->type) {
    case TMIDL_ITEM_OPAQUE:
        printf("Type: Opaque\n");
        break;
    case TMIDL_ITEM_INTERFACE:
        printf("Type: Interface\n");
        break;
    }

    for (size_t i = 0; i < declaration->functions_count; i++) {
        printf("Function: %s\n", declaration->functions[i]->name);
    }

    printf("\n");
}

static void on_module_doc(const char *doc, void *user_context)
{
    printf("Doc: %s\n", doc);
}

static void on_diagnostic(const tmidl_diagnostic_t *diagnostic, void *user_context)
{
    const char *level = "WARN";

    if (diagnostic->level == TMIDL_LEVEL_ERROR) {
        level = "ERROR";
    }

    printf("%s: %s\n", level, diagnostic->message);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("File to read not specified!");
        return 1;
    }
    if (argc > 2) {
        printf("Too many arguments!\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        printf("Failed to open target file!\n");
        return 1;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read the entire file to memory
    char *data = malloc(size + 1);
    fread(data, 1, size, file);
    fclose(file);
    data[size] = 0;

    // Validate the file did not contain any nulls, as those make the string invalid
    for (size_t i = 0; i < size; i++) {
        if (data[i] == 0) {
            printf("File contained null, not allowed.");
            free(data);
            return 1;
        }
    }

    // Parse the file
    tmidl_callbacks_i callbacks = {
        .on_declaration = on_declaration,
        .on_module_doc = on_module_doc,
        .on_diagnostic = on_diagnostic
    };
    tmidl_parser_o *parser = tmidl_parser_create();
    bool success = tmidl_parser_parse(parser, data, &callbacks, NULL);
    tmidl_parser_destroy(parser);
    free(data);

    if (!success) {
        printf("Parsing failed!\n");
        return 1;
    }

    return 0;
}
