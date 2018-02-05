#include "lispp.h"
#include <exception>
#include <cctype>
#include <cstdio>
#include <iostream>
#include <string>

Tokenizer::Tokenizer(std::istream *input_stream)
        : input_stream_(input_stream) {}

void Tokenizer::ReadNext() {
    if (input_stream_->peek() == EOF) {
        type_ = TokenType::END_OF_FILE;
        return;
    }

    *input_stream_ >> std::ws;

    char symb;
    *input_stream_ >> symb;
    auto token = std::string();
    token.push_back(symb);

    if (!(symb == '(' || symb == ')')) {
        auto next = input_stream_->peek();
        while (next != '\n' && next != ' ' && next != '(' && next != ')' && next != EOF) {
            token.push_back(input_stream_->get());
            next = input_stream_->peek();
        }
    } else {
        type_ = ((symb == '(') ? TokenType::OPEN_PARENT : TokenType::CLOSE_PARENT);
        return;
    }

    if (IsNumber(token)) {
        size_t *processed = nullptr;
        number_ = std::stoll(token, processed, 10);
        type_ = TokenType::NUMBER;
    } else if (IsBuiltin(token)) {
        name_ = token;
        type_ = TokenType::BUILTIN;
    } else if (IsName(token)) {
        name_ = token;
        type_ = TokenType::NAME;
    } else {
        switch (symb) {
            case '(':
                type_ = TokenType::OPEN_PARENT;
                break;
            case ')':
                type_ = TokenType::CLOSE_PARENT;
                break;
            case '.':
                type_ = TokenType::PAIR;
                break;
            case '\'':
                type_ = TokenType::APOSTROPH;
                break;
            default:
                type_ = TokenType::UNKNOWN;
                break;
        }
    }
}

Tokenizer::TokenType Tokenizer::ShowTokenType() {
    return type_;
}

const std::string &Tokenizer::GetTokenName() {
    return name_;
}

int64_t Tokenizer::GetTokenNumber() {
    return number_;
}

bool Tokenizer::IsNumber(const std::string &token) {
    try {
        size_t processed = 0;
        std::stoll(token, &processed, 10);
        if (processed != token.size()) {
            return false;
        }
    } catch (const std::invalid_argument &exc) {
        return false;
    } catch (const std::out_of_range &exc) {
        return false;
    }

    return true;
}

bool Tokenizer::IsName(const std::string &token) {
    bool return_value = false;
    if (builtins_.find(token) != builtins_.end()) {
        return_value = true;
    }

    if (!token.empty() && std::isalpha(token[0])) {
        auto iter = token.begin();
        while (iter < token.end()) {
            if (!std::isalnum(*iter)) {
                break;
            }
            ++iter;
        }
        if (iter == token.end()) {
            return_value = true;
        }
    }

    return return_value;
}

bool Tokenizer::IsBuiltin(const std::string &token) {
    return builtins_.find(token) != builtins_.end();
}

Pair::Pair()
        : child(nullptr), next(nullptr) {}

AST::AST(std::istream *input_stream)
        : Tokenizer(input_stream) {}

void AST::InsertLexema() {
    ReadNext();

    curr_->type = ShowTokenType();

    switch (curr_->type) {
        case TokenType::OPEN_PARENT:
#ifdef TEST__DUMP
            TEST_StatusDump();
#endif
            return_stack_.push_back(curr_);
            TurnDown();
            break;

        case TokenType::CLOSE_PARENT:
#ifdef TEST__DUMP
            TEST_StatusDump();
#endif
            curr_ = return_stack_.back();
            return_stack_.pop_back();
            TurnNext();
            break;

        case TokenType::NUMBER:
            curr_->value = GetTokenNumber();
#ifdef TEST__DUMP
            TEST_StatusDump();
#endif
            TurnNext();
            break;

        case TokenType::NAME:
            curr_->value = GetTokenName();
#ifdef TEST__DUMP
            TEST_StatusDump();
#endif
            TurnNext();
            break;

        case TokenType::BUILTIN:
            curr_->value = GetTokenName();
#ifdef TEST__DUMP
            TEST_StatusDump();
#endif
            TurnNext();
            break;

        default:
            break;
    }
}

inline void AST::TurnNext() {
    curr_->next = std::make_shared<Pair>();
    curr_ = curr_->next;
}

inline void AST::TurnDown() {
    curr_->child = std::make_shared<Pair>();
    curr_ = curr_->child;
}

void AST::TEST_StatusDump() {
    std::cout << "type " << static_cast<int>(curr_->type) << std::endl;
    switch (curr_->type) {
        case TokenType::OPEN_PARENT:
            std::cout << "OPEN_PARENT => no value" << std::endl;
            break;
        case TokenType::CLOSE_PARENT:
            std::cout << "CLOSE_PARENT => no value" << std::endl;
            break;
        case TokenType::NUMBER:
            std::cout << "value: " << curr_->value.TakeValue<int64_t>() << std::endl;
            break;
        case TokenType::NAME:
            std::cout << "value: " << curr_->value.TakeValue<std::string>() << std::endl;
            break;
        case TokenType::BUILTIN:
            std::cout << "value: " << curr_->value.TakeValue<std::string>() << std::endl;
        default:
            std::cout << "value: default" << std::endl;
            break;
    }
    std::cout << "child " << curr_->child << std::endl;
    std::cout << "next " << curr_->next << std::endl << std::endl;
}

int64_t AST::Evaluate(std::shared_ptr<Pair> curr) {
    if (curr == nullptr) {
        return Evaluate(root_);
    }

    if (curr->child != nullptr) {
        curr->value = Evaluate(curr->child);
    }

    if (curr->type == TokenType::NAME) {

    }

    return curr->value.TakeValue<int64_t>();
}

int64_t Add(std::shared_ptr<Pair> curr) {
    if (curr->next != nullptr) {
        return curr->value.TakeValue<int64_t>() + Add(curr->next);
    }
    return curr->value.TakeValue<int64_t>();
}
