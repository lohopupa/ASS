#ifndef LEXER_H_
#define LEXER_H_

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    KEYWORD,
    SYMBOL,
    PUNCT,
    FLOAT_NUMBER,
    INT_NUMBER,
    END,

    TOKEN_TYPES_COUNT
} TokenType;

static char *TokenString[] = {
    [KEYWORD] = "KEYWORD",       [SYMBOL] = "SYMBOL",
    [PUNCT] = "PUNCT",           [FLOAT_NUMBER] = "FLOAT_NUMBER",
    [INT_NUMBER] = "INT_NUMBER", [END] = "END"};
static_assert(TOKEN_TYPES_COUNT == sizeof(TokenString) / sizeof(TokenString[0]),
              "Amount of Token types has changed");

typedef enum {
    KEY_FOR,
    KEY_IF,
    KEY_ELSE,
    KEY_WHILE,
    KEY_PRINT,
    KEYWORDS_COUNT
} KewordTypes;

static char *Keywords[] = {
    [KEY_FOR] = "for",
    [KEY_IF] = "if",
    [KEY_ELSE] = "else",
    [KEY_WHILE] = "while",
    [KEY_PRINT] = "print",
};
static_assert(KEYWORDS_COUNT == sizeof(Keywords) / sizeof(Keywords[0]),
              "Amount of Keword types has changed");

typedef enum {
    // Longer puncts should be higher in list
    PUNCT_INC,
    PUNCT_DEC,
    PUNCT_EQUALS,
    PUNCT_LESS_OR_EQ,
    PUNCT_GREATER_OR_EQ,
    //
    PUNCT_SEMICOLON,
    PUNCT_COMMA,
    PUNCT_ASSIGN,
    PUNCT_LESS,
    PUNCT_GREATER,
    PUNCT_PLUS,
    PUNCT_MINUS,
    PUNCT_ASTERIK,
    PUNCT_SlASH,
    PUNCT_OPEN_PAREN,
    PUNCT_CLOSE_PAREN,
    PUNCT_OPEN_CURLY,
    PUNCT_CLOSE_CURLY,
    PUNCTS_COUNT
} PunctTypes;

static char *Puncts[] = {
    [PUNCT_DEC] = "--",       [PUNCT_INC] = "++",
    [PUNCT_PLUS] = "+",       [PUNCT_MINUS] = "-",
    [PUNCT_ASTERIK] = "*",    [PUNCT_SlASH] = "/",
    [PUNCT_OPEN_PAREN] = "(", [PUNCT_CLOSE_PAREN] = ")",
    [PUNCT_EQUALS] = "==",    [PUNCT_ASSIGN] = "=",
    [PUNCT_COMMA] = ",",      [PUNCT_SEMICOLON] = ";",
    [PUNCT_LESS] = "<",       [PUNCT_LESS_OR_EQ] = "<=",
    [PUNCT_GREATER] = ">",    [PUNCT_GREATER_OR_EQ] = ">=",
    [PUNCT_OPEN_CURLY] = "{", [PUNCT_CLOSE_CURLY] = "}"};
static_assert(PUNCTS_COUNT == sizeof(Puncts) / sizeof(Puncts[0]),
              "Amount of Punct types has changed");

typedef struct {
    const char *path;
    int line;
    int column;
} Location;

typedef struct {
    TokenType type;
    char *value;
    Location loc;
} Token;

typedef struct {
    const char *data;
    const char *filepath;
    size_t length;
    size_t cursor;
    size_t line;
    size_t column;
} Lexer;

char next_char(Lexer *l);
char peek_char(Lexer *l);
bool skip_chars(Lexer *l, unsigned int n);
bool check_prefix(Lexer *l, const char *prefix);
void init_lexer(Lexer *l, const char *filepath, const char *data,
                size_t content_length);
void skip_whitespace(Lexer *l);
bool next_token(Lexer *l, Token *t);
bool peek_token(Lexer *l, Token *t);

#endif // LEXER_H_

#pragma message("LEXER_IMPLEMENTATION should be deleted.")
#define LEXER_IMPLEMENTATION
#ifdef LEXER_IMPLEMENTATION

void init_lexer(Lexer *l, const char *filepath, const char *data,
                size_t content_length) {
    l->data = data;
    l->filepath = filepath;
    l->cursor = 0;
    l->line = 1;
    l->column = 1;
    l->length = content_length;
    return;
}
bool check_prefix(Lexer *l, const char *prefix) {
    size_t len = strlen(prefix);
    if (l->cursor + len > l->length)
        return false;
    for (size_t i = 0; i < len; i++) {
        if (l->data[l->cursor + i] != prefix[i])
            return false;
    }
    return true;
}

bool skip_chars(Lexer *l, unsigned int n) {
    if (l->cursor + n > l->length)
        return false;
    for (int i = 0; i < n; i++) {
        next_char(l);
    }
    return true;
}
char peek_char(Lexer *l) {
    if (l->cursor >= l->length)
        return '\0';
    return l->data[l->cursor];
}

char next_char(Lexer *l) {
    if (l->cursor >= l->length)
        return '\0';
    if (l->data[l->cursor] == '\n') {
        l->column = 1;
        l->line++;
    } else {
        l->column++;
    }
    return l->data[++l->cursor];
}

bool peek_token(Lexer *l, Token *t) { return false; }

bool next_token(Lexer *l, Token *t) {
    skip_whitespace(l);

    for (size_t i = 0; i < PUNCTS_COUNT; i++) {
        // TODO: combine check_prefix & skip_chars
        if (check_prefix(l, Puncts[i])) {
            t->loc = (Location){
                .column = l->column, .line = l->line, .path = l->filepath};
            t->type = PUNCT;
            t->value = Puncts[i];
            skip_chars(l, strlen(Puncts[i]));
            return true;
        }
    }

    char next = peek_char(l);
    // TODO: Remove buffer
    if (next == '#') {
        while (l->data[l->cursor] != '\n')
            next_char(l);
        // skip_whitespace(l);
    }
    size_t x = 0;
    if (isalpha(next)) {
        while (isalnum(l->data[l->cursor + x]))
            x++;
        t->type = SYMBOL;
    } else if (isdigit(next)) {
        while (isdigit(l->data[l->cursor + x]))
            x++;
        if (l->data[l->cursor + x] == '.') {
            t->type = FLOAT_NUMBER;
            x++;
            while (isdigit(l->data[l->cursor + x]))
                x++;
        } else {
            t->type = INT_NUMBER;
        }
    } else {
        return false;
    }
    t->loc =
        (Location){.column = l->column, .line = l->line, .path = l->filepath};
    t->value = malloc(x);
    sprintf(t->value, "%.*s", x, l->data + l->cursor);
    if(t->type == SYMBOL) {
        for(size_t i = 0; i < KEYWORDS_COUNT; i++){
            if(strcmp(t->value, Keywords[i]) == 0) {
                t->type = KEYWORD;
                break;
            }
        }
    }
    skip_chars(l, x);
    return true;
}

void skip_whitespace(Lexer *l) {
    // const char *data = l->data + l->cursor;
    while (isspace(peek_char(l)))
        next_char(l);
}

#endif // LEXER_IMPLEMENTATION