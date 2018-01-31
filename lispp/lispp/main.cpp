#include <iostream>
// #include <histedit.h>
#include <sstream>
#include <cstdio>
#include "lispp.h"


int main() {
    std::cout << '\n' - 0;
    std::stringstream string_stream("(+ 12003 (* 2 3))");
    AST ast(&string_stream);
    for (int i = 0; i < 9; ++i) {
        ast.InsertLexema();
    }
    

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
