#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include <sstream>
#include "lexer.h"
#include "token.h"

std::vector<Token> make_token_vec(std::string data) {
    // split all characters that have meanings
    std::regex delimiterRegex("([\\\\:;\\n\\s\"*+\\-/()[\\]{}'=<>!?,%]|[^\\\\:;\\n\\s\"*+\\-/()[\\]{}'=<>!?,%]+)");

    std::sregex_token_iterator it(data.begin(), data.end(), delimiterRegex, 1);
    std::sregex_token_iterator end;

    std::vector<std::string> regex_split(it, end);

    // discard comments
    bool in_comment = false;
    bool in_multiline = false;
    std::string previous = "";
    std::vector<std::string> comment_split = std::vector<std::string>();
    for (int i = 0; i < regex_split.size(); i++) {
        if (in_multiline) {
            if (!previous.compare("*") && !regex_split[i].compare("/")) { // multiline comment ends with */
                in_multiline = false;
            }
            previous = regex_split[i];
            continue;
        }
        else if (in_comment) {
            if (!regex_split[i].compare("\n")) { // single line comment end with a newline
                in_comment = false;
            }
            previous = regex_split[i];
            continue;
        }
        else if (!previous.compare("/") && !regex_split[i].compare("/")) { // single line comments start with //
            in_comment = true;
            comment_split.pop_back();
            previous = regex_split[i];
            continue;
        }
        else if (!previous.compare("/") && !regex_split[i].compare("*")) { // multiline comments start with /*
            in_multiline = true;
            comment_split.pop_back();
            previous = regex_split[i];
            continue;
        }
        comment_split.push_back(regex_split[i]);
        previous = regex_split[i];
    }

    regex_split.clear(); // not going to access this anymore
    regex_split.shrink_to_fit();

    // piece together stuff between " and '
    bool in_string = false;
    bool in_char = false;
    int line_number = 1;
    int col_number = 1;
    std::string buffer = "";
    std::vector<std::string> quote_split = std::vector<std::string>();
    for (int i = 0; i < comment_split.size(); i++) {
        col_number += comment_split[i].size();
        if (!comment_split[i].compare("\n")) {
            line_number++;
        }
        if (in_string) {
            buffer.append(comment_split[i]);
            if (!comment_split[i].compare("\"")) {
                in_string = false;
                quote_split.push_back(buffer);
                buffer = "";
            }
            continue;
        }
        else if (in_char) {
            buffer.append(comment_split[i]);
            if (!comment_split[i].compare("'")) {
                in_char = false;
                quote_split.push_back(buffer);
                buffer = "";
            }
        }
        else if (!comment_split[i].compare("\"")) {
            in_string = true;
            buffer.append(comment_split[i]);
            continue;
        }
        else if (!comment_split[i].compare("'")) {
            in_char = true;
            buffer.append(comment_split[i]);
            continue;
        }
        quote_split.push_back(comment_split[i]);
    }
    if (!buffer.empty()) {
        throw LexError("bluefox_lang: " + std::to_string(line_number) + ", " + std::to_string(col_number) + ": Unclosed delimiter: '\"'");
    }

    comment_split.clear();
    comment_split.shrink_to_fit();

    // remove all singleton spaces and newlines
    std::vector<std::string> singleton_split = std::vector<std::string>();
    for (int i = 0; i < quote_split.size(); i++) {
        if (!quote_split[i].compare(" ")) { // since we grouped spaces inside " and ' they will not be caught by this if statement
            continue;
        }
        else if (!quote_split[i].compare("\n")) { // same with newlines
            continue;
        }
        singleton_split.push_back(quote_split[i]);
    }

    quote_split.clear();
    quote_split.shrink_to_fit();

    std::vector<std::string> multichar_split = std::vector<std::string>();
    for (int i = 0; i < singleton_split.size(); i++) {
        std::string current = singleton_split[i];
        std::string next = (i + 1 < singleton_split.size()) ? singleton_split[i + 1] : "";
        std::string nextnext = (i + 2 < singleton_split.size()) ? singleton_split[i + 2] : "";
        std::string combined = current + next;
        std::string combined_next = combined + nextnext;

        if (combined == "->" ||   // returns
            combined == "&&" ||   // and
            combined == "||" ||   // or
            combined == "==" ||   // equal
            combined == "!=" ||   // not equal
            combined == ">=" ||   // greater or equal
            combined == "<=" ||   // less or equal
            combined == "<<" ||   // bitwise shift left
            combined == "::" ||   // scope operator
            combined == ">>" ||   // bitwise shift right
            combined == "+=" ||   // add and set
            combined == "-=" ||   // subtract and set
            combined == "*=" ||   // multiply and set
            combined == "/=" ||   // divide and set
            combined == "%=" ||   // modulo and set
            combined == "&=" ||   // bitwise and and set
            combined == "|=" ||   // bitwise or and set
            combined == "^=" ||   // bitwise xor and set
            combined_next == "<<=" ||  // bitwise shift left and set
            combined_next == ">>="     // bitwise shift right and set
        ) {
            if (combined_next == "<<=" || combined_next == ">>=") {
                multichar_split.push_back(combined_next);
                i += 2; // increment i because we've processed a three character operator
            } else {
                multichar_split.push_back(combined);
                i++; // increment i because we've processed a two character operator
            }
        } else {
            multichar_split.push_back(current);
        }
    }

    singleton_split.clear();
    singleton_split.shrink_to_fit();


    std::vector<Token> out = std::vector<Token>();
    for (int i = 0; i < multichar_split.size(); i++) {
        std::string token = multichar_split[i];
        if (!token.compare("+") ||   // add
            !token.compare("=") ||   // set
            !token.compare("->") ||  // returns
            !token.compare("-") ||   // subtract
            !token.compare("*") ||   // multiply
            !token.compare("/") ||   // divide
            !token.compare("%") ||   // modulo
            !token.compare(":") ||   // type
            !token.compare("::") ||  // scope operator
            !token.compare("&&") ||  // and
            !token.compare("||") ||  // or
            !token.compare("!") ||   // not
            !token.compare("==") ||  // equal
            !token.compare("!=") ||  // not equal
            !token.compare(">") ||   // greater
            !token.compare("<") ||   // less
            !token.compare(">=") ||  // greater or equal
            !token.compare("<=") ||  // less or equal
            !token.compare("<<") ||  // bitwise shift left
            !token.compare(">>") ||  // bitwise shift right
            !token.compare("^") ||   // bitwise xor
            !token.compare("|") ||   // bitwise or
            !token.compare("&") ||   // bitwise and
            !token.compare("+=") ||  // add and set
            !token.compare("-=") ||  // subtract and set
            !token.compare("*=") ||  // multiply and set
            !token.compare("/=") ||  // divide and set
            !token.compare("%=") ||  // modulo and set
            !token.compare("&=") ||  // bitwise and and set
            !token.compare("|=") ||  // bitwise or and set
            !token.compare("^=") ||  // bitwise xor and set
            !token.compare("<<=") || // bitwise shift left and set
            !token.compare(">>=")    // bitwise shift rifht and set
        ) {
            out.push_back(Token(TOKEN_OPERATOR, token));
            continue;
        }
        if (!token.compare("var") ||
            !token.compare("val") ||
            !token.compare("fn") ||
            !token.compare("int") ||
            !token.compare("float") ||
            !token.compare("double") ||
            !token.compare("bool") ||
            !token.compare("char") ||
            !token.compare("string") ||
            !token.compare("true") ||
            !token.compare("false") ||
            !token.compare("if") ||
            !token.compare("else") ||
            !token.compare("while") ||
            !token.compare("loop") ||
            !token.compare("for") ||
            !token.compare("return") ||
            !token.compare("break") ||
            !token.compare("continue") ||
            !token.compare("class") ||
            !token.compare("struct") ||
            !token.compare("public") ||
            !token.compare("private") ||
            !token.compare("protected") ||
            !token.compare("static") ||
            !token.compare("final") ||
            !token.compare("const") ||
            !token.compare("enum") ||
            !token.compare("throw") ||
            !token.compare("try") ||
            !token.compare("err") ||
            !token.compare("catch") ||
            !token.compare("mutable") ||
            !token.compare("virtual") ||
            !token.compare("this") ||
            !token.compare("delete") ||
            !token.compare("use") ||
            !token.compare("namespace") ||
            !token.compare("sizeof") ||
            !token.compare("alignas") ||
            !token.compare("alignof") ||
            !token.compare("typeid") ||
            !token.compare("constexpr") ||
            !token.compare("thread_local") ||
            !token.compare("noexcept") ||
            !token.compare("null") ||
            !token.compare("import") ||
            !token.compare("module")
        ) {
            out.push_back(Token(TOKEN_KEYWORD, token));
            continue;
        }
        std::stringstream ss(token);
        double test_double;
        if (ss >> test_double) {
            out.push_back(Token(TOKEN_NUMBER, token));
            continue;
        } 
        if (token[0] == '\'' && token[token.length()-1] == '\'') {
            out.push_back(Token(TOKEN_CHAR, token));
            continue;
        }
        if (token[0] == '"' && token[token.length()-1] == '"') {
            out.push_back(Token(TOKEN_STRING, token));
            continue;
        }
        if (!token.compare("(")) {
            out.push_back(Token(TOKEN_L_PAREN, token));
            continue;
        }
        if (!token.compare(")")) {
            out.push_back(Token(TOKEN_R_PAREN, token));
            continue;
        }
        if (!token.compare("[")) {
            out.push_back(Token(TOKEN_L_BRACKET, token));
            continue;
        }
        if (!token.compare("]")) {
            out.push_back(Token(TOKEN_R_BRACKET, token));
            continue;
        }
        if (!token.compare("{")) {
            out.push_back(Token(TOKEN_L_BRACE, token));
            continue;
        }
        if (!token.compare("}")) {
            out.push_back(Token(TOKEN_R_BRACE, token));
            continue;
        }
        if (!token.compare(";")) {
            out.push_back(Token(TOKEN_SEMICOLON, token));
            continue;
        }
        if (!token.compare(",")) {
            out.push_back(Token(TOKEN_COMMA, token));
            continue;
        }
        std::regex identifierRegex("^[a-zA-Z_]+$");
        if (std::regex_match(token, identifierRegex)) {
            out.push_back(Token(TOKEN_IDENTIFIER, token));
            continue;
        }
        out.push_back(Token(TOKEN_NONE, token));
    }

    return out;
}