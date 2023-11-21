#pragma once
#ifdef __cplusplus
#ifndef BLUEFOX_TOKEN
#include <string>

enum TokenType {
    TOKEN_NONE,
    TOKEN_OPERATOR,
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_CHAR,
    TOKEN_STRING,
    TOKEN_L_PAREN,
    TOKEN_R_PAREN,
    TOKEN_L_BRACKET,
    TOKEN_R_BRACKET,
    TOKEN_L_BRACE,
    TOKEN_R_BRACE,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
};
std::string token_type_to_string(TokenType tokenType);

struct Token {
    TokenType type;
    std::string value;

    Token(TokenType t, std::string v) : type(t), value(v) {}
};
std::string token_to_string(Token token);

#define BLUEFOX_TOKEN
#endif // BLUEFOX_TOKEN
#endif // __cplusplus