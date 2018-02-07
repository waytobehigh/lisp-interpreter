#include <iostream>
#include <sstream>
#include <cstdio>
#include <cassert>
#include "lispp.h"

void PrintResult(const AST::Pair& result) {
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

std::string ResToStr(const AST::Pair& result) {
    switch (result.type) {
        case Tokenizer::TokenType::NUM:
            return std::to_string(result.value.TakeValue<int64_t>());
        case Tokenizer::TokenType::BOOL:
            return ((result.value.TakeValue<int64_t>()) ? "#t" : "#f");
        default:
            break;
    }
}

void TEST(const std::string& expr, const std::string& ans) {
    auto string_stream = std::stringstream(expr);
    auto ast = AST(&string_stream);
    while (ast.InsertLexema()) {}

    auto res = ResToStr(ast.Evaluate(nullptr));

    if (res != ans) {
        std::cerr << "TEST FAILED: " + expr + " must be " + ans + " but got " + res;
        std::cerr << std::endl;
    }
}

int main() {
    /* Output tests */
    TEST("#f", "#f");
    TEST("10", "10");

    /* Arithmetical tests */
    TEST("(+ 1 2)", "3");
    TEST("(+ (* 1 2 3) (- 2 3) (/ 4 2))", "7");
    TEST("(+ 800 (- 100 230 (* 21 31 (/ 10 (- 3 2) 10))))", "19");

    /* Builtins tests */
    TEST("(abs -100)", "100");
    TEST("(abs 241)", "241");
    TEST("(max 1 2)", "2");



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
