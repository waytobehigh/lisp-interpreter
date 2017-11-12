#include <iostream>
// #include <histedit.h>
#include <sstream>
#include <cstdio>
#include "lispp.h"


int main() {
    std::stringstream string_stream("(+ 1 2)");
    AST ast(&string_stream);
    ast.InsertLexema();
    ast.InsertLexema();
    ast.InsertLexema();
    ast.InsertLexema();
    ast.InsertLexema();

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
