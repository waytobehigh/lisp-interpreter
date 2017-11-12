#include <iostream>
// #include <histedit.h>
#include "lispp.h"
#include <sstream>
#include <cstdio>

int main() {
    std::stringstream string_stream("(+ 1 (lambda (x)( cdr '(100 2)))");
    Tokenizer tokenizer(&string_stream);

    tokenizer.ReadNext();
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

    return 0;
}
