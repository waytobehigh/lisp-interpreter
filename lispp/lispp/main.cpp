#include <iostream>
// #include <histedit.h>
#include "lispp.h"
#include <sstream>

int main() {
    std::stringstream string_stream("213)) 312 2asd");
    Tokenizer tokenizer(&string_stream);

    tokenizer.ReadNext();
    while (tokenizer.ShowTokenType() != Tokenizer::TokenType::END_OF_FILE) {
        if (tokenizer.ShowTokenType() == Tokenizer::TokenType::NUMBER) {
            std::cout << tokenizer.GetTokenNumber() << ' ';
        } else if (tokenizer.ShowTokenType() == Tokenizer::TokenType::NAME) {
            std::cout << tokenizer.GetTokenName() << ' ';
        } else if (tokenizer.ShowTokenType() == Tokenizer::TokenType::CLOSE_PARENT) {
            std::cout << ') ';
        }
        tokenizer.ReadNext();
    }

    return 0;
}
