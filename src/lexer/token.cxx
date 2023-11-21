#include <string>
#include "token.h"

std::string token_type_to_string(TokenType tokenType) {
    switch(tokenType) {
        case TOKEN_NONE:
            return "TOKEN_NONE";
        case TOKEN_OPERATOR:
            return "TOKEN_OPERATOR";
        case TOKEN_KEYWORD:
            return "TOKEN_KEYWORD";
        case TOKEN_IDENTIFIER:
            return "TOKEN_IDENTIFIER";
        case TOKEN_NUMBER:
            return "TOKEN_NUMBER";
        case TOKEN_CHAR:
            return "TOKEN_CHAR";
        case TOKEN_STRING:
            return "TOKEN_STRING";
        case TOKEN_L_PAREN:
            return "TOKEN_L_PAREN";
        case TOKEN_R_PAREN:
            return "TOKEN_R_PAREN";
        case TOKEN_L_BRACKET:
            return "TOKEN_L_BRACKET";
        case TOKEN_R_BRACKET:
            return "TOKEN_R_BRACKET";
        case TOKEN_L_BRACE:
            return "TOKEN_L_BRACE";
        case TOKEN_R_BRACE:
            return "TOKEN_R_BRACE";
        case TOKEN_SEMICOLON:
            return "TOKEN_SEMICOLON";
        case TOKEN_COMMA:
            return "TOKEN_COMMA";
        default:
            return "UNKNOWN_TOKEN";
    }
}

std::string token_to_string(Token token) {
    return token_type_to_string(token.type) + ": " + token.value;
}