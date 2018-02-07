#include <iostream>
#include <sstream>
#include <cstdio>
#include "lispp.h"

const std::string& PrintResult(const AST::Pair& result) {
    switch (result.type) {
        case Tokenizer::TokenType::NUM:
            std::cout << result.value.TakeValue<int64_t>();
            break;
        case Tokenizer::TokenType::BOOL:
            std::cout << ((result.value.TakeValue<int64_t>()) ? "#t" : "#f");
            break;
        default:
            break;
    }
    std::cout << std::endl;
}

int main() {
    std::stringstream string_stream("(abs 1)");
    AST ast(&string_stream);
    while (ast.InsertLexema()) {}

    PrintResult(ast.Evaluate(nullptr));

    //tokenizer.ReadNext();
    /*
    while (tokenizer.ShowTokenType() != Tokenizer::TokenType::END_OF_FILE) {
        if (tokenizer.ShowTokenType() == Tokenizer::TokenType::NUM) {
            std::cout << tokenizer.GetTokenNumber() << ' ';
        } else if (tokenizer.ShowTokenType() == Tokenizer::TokenType::NAME) {
            std::cout << tokenizer.GetTokenName() << ' ';
        } else if (tokenizer.ShowTokenType() == Tokenizer::TokenType::CLOSE_PARENT) {
            std::cout << ") ";
        } else if (tokenizer.ShowTokenType() == Tokenizer::TokenType::OPEN_PARENT) {
            std::cout << "( ";
        } else if (tokenizer.ShowTokenType() == Tokenizer::TokenType::APOSTROPH) {
            std::cout << "\' ";
        }
        tokenizer.ReadNext();
    }
    */

    return 0;
}
