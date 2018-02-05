#include <iostream>
// #include <histedit.h>
#include <sstream>
#include <cstdio>
#include "lispp.h"


int main() {
    std::stringstream string_stream("(+ (- 8 1 2 3) (* 1 (/ 10 5)))");
    AST ast(&string_stream);
    while (ast.InsertLexema()) {}

    std::cout << ast.Evaluate(nullptr) << std::endl;

    //tokenizer.ReadNext();
    /*
    while (tokenizer.ShowTokenType() != Tokenizer::TokenType::END_OF_FILE) {
        if (tokenizer.ShowTokenType() == Tokenizer::TokenType::NUMBER) {
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
