#include <stdbool.h>
#include <stdio.h>

#define DA_IMPLEMENTATION
#include "./da.h"

#define LEXER_IMPLEMENTATION
#include "./lexer.h"

// TODO: Get rid of DA
// TODO: Combine lexer and parser or define parameters here

int main() {
    char *filepath = "./test";
    FILE *f = fopen(filepath, "r");
    if (f == NULL) {
        printf("[ERROR]: Could not open file %s\n", filepath);
        return -1;
    }
    if (fseek(f, 0, SEEK_END) < 0) {
        printf("[ERROR]: Could not read bytes");
        return -1;
    }
    long m = ftell(f);
    if (m < 0) {
        printf("[ERROR]: Could not read size of file");
        return -1;
    }
    if (fseek(f, 0, SEEK_SET) < 0) {
        printf("[ERROR]: Could not return carret");
        return -1;
    }

    DA content = DA_create();
    DA_extend(&content, m);
    fread(content.items + content.count, m, 1, f);
    content.count += m;
    Lexer *l = malloc(sizeof(Lexer));
    Token *t = malloc(sizeof(Token));
    init_lexer(l, filepath, content.items, content.count);
    
    while (next_token(l, t)) {
        printf("%s:%d:%d: TYPE: %s VALUE: %s\n", t->loc.path, t->loc.line,
               t->loc.column, TokenString[t->type], t->value);
    }
}