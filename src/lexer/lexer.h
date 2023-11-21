#pragma once
#ifdef __cplusplus
#ifndef BLUEFOX_LEXER
#include <vector>
#include <stdexcept>
#include "token.h"

class LexError : public std::runtime_error {
public:
    explicit LexError(const char* message) : std::runtime_error(message) { }
    explicit LexError(const std::string& message) : std::runtime_error(message) { }
};


std::vector<Token> make_token_vec(std::string data);

#endif // BLUEFOX_LEXER
#endif // __cplusplus