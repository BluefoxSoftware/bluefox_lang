#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "lexer/lexer.h"
#include "lexer/token.h"

int main(int argc, char** argv) {
    if (!(argc >= 2)) {
        std::cout << "no source files given" << std::endl;
    }
    std::string source_file = argv[1];
    std::ifstream in_file(source_file);
    std::stringstream str_stream;
    str_stream << in_file.rdbuf();
    std::string data = str_stream.str();

    // token vector
    std::vector<Token> tokens = make_token_vec(data);
    for (int i = 0; i < tokens.size(); i++) {
        std::cout << "[" << token_to_string(tokens[i]) << "]" << std::endl;
    }

    return 0;
}