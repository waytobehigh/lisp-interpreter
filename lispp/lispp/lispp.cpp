#include <tic.h>
#include "lispp.h"

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
        type_ = TokenType::NUM;
    } else if (IsBool(token)) {
        number_ = bools_.at(token);
        type_ = TokenType::BOOL;
    } else if (IsBuiltin(token)) {
        name_ = token;
        type_ = TokenType::BUILTIN;
    } else if (IsName(token)) {
        name_ = token;
        type_ = TokenType::NAME;
    } else {
        switch (symb) {
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

bool Tokenizer::IsBool(const std::string &token) {
    return bools_.find(token) != bools_.end();
}

AST::Pair::Pair()
        : value(std::shared_ptr<Pair>(nullptr)), next(nullptr) {}

AST::AST(std::istream *input_stream)
        : Tokenizer(input_stream) {}

std::shared_ptr<AST::Pair> AST::InsertLexema() {
    ReadNext();

    curr_->type = ShowTokenType();

    if (curr_->type == TokenType::END_OF_FILE) {
        return nullptr;
    }

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

        case TokenType::NUM:
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

        case TokenType::BOOL:
            curr_->value = GetTokenNumber();
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

    return curr_;
}

inline void AST::TurnNext() {
    curr_->next = std::make_shared<Pair>();
    curr_ = curr_->next;
}

inline void AST::TurnDown() {
    curr_->value = std::make_shared<Pair>();
    curr_ = curr_->value.TakeValue<std::shared_ptr<Pair>>();
}

void AST::TEST_StatusDump() {
    std::cout << "type: ";
    switch (curr_->type) {
        case TokenType::OPEN_PARENT:
            std::cout << "OPEN_PARENT" << std::endl;
            std::cout << "no value" << std::endl;
            break;
        case TokenType::CLOSE_PARENT:
            std::cout << "CLOSE_PARENT" << std::endl;
            std::cout << "no value" << std::endl;
            break;
        case TokenType::NUM:
            std::cout << "NUM" << std::endl;
            std::cout << "value: " << curr_->value.TakeValue<int64_t>() << std::endl;
            break;
        case TokenType::NAME:
            std::cout << "NAME" << std::endl;
            std::cout << "value: " << curr_->value.TakeValue<std::string>() << std::endl;
            break;
        case TokenType::BUILTIN:
            std::cout << "BUILTIN" << std::endl;
            std::cout << "value: " << curr_->value.TakeValue<std::string>() << std::endl;
            break;
        case TokenType::BOOL:
            std::cout << "BOOL" << std::endl;
            std::cout << "value: " << curr_->value.TakeValue<int64_t>() << std::endl;
            break;
        default:
            std::cout << "DEFAULT" << std::endl;
            std::cout << "no value" << std::endl;
            break;
    }
    std::cout << "next " << curr_->next << std::endl << std::endl;
}

const AST::Pair& AST::Evaluate(std::shared_ptr<Pair> curr) {
    if (curr == nullptr) {
        return Evaluate(root_);
    }

    switch (curr->type) {
        case TokenType::OPEN_PARENT: {
            auto res = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
            curr->value = res.value;
            curr->type = res.type;
        }
            break;
        case TokenType::BUILTIN:
            switch (Tokenizer::builtins_.at(curr->value.TakeValue<std::string>())) {
                case 20: // '+'
                    curr->value = Add(curr);
                    curr->type = TokenType::NUM;
                    break;
                case 21: // '-'
                    curr->value = Sub(curr);
                    curr->type = TokenType::NUM;
                    break;
                case 22: // '*'
                    curr->value = Mul(curr);
                    curr->type = TokenType::NUM;
                    break;
                case 23: // '/'
                    curr->value = Div(curr);
                    curr->type = TokenType::NUM;
                    break;
                case 24: // '='
                    curr->value = EQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 25: // '>'
                    curr->value = GT(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 26: // '<'
                    curr->value = LT(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 27: // '>='
                    curr->value = GEQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 28: // '<='
                    curr->value = LEQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 29: // 'min'
                    curr->value = Min(curr);
                    curr->type = TokenType::NUM;
                    break;
                case 30: // 'max'
                    curr->value = Max(curr);
                    curr->type = TokenType::NUM;
                    break;
                case 31: // 'abs'
                    curr->value = Abs(curr);
                    curr->type = TokenType::NUM;
                    break;
                case 8: // 'null?'
                    curr->value = is_null(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 9: // 'pair?'
                    curr->value = is_pair(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 10: // 'number?'
                    curr->value = is_number(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case 11: // 'boolean?'
                    curr->value = is_bool(curr);
                    curr->type = TokenType::BOOL;
                    break;
                default:
                    break;
            }
            break;
        case TokenType::NAME:
            break;
        default:
            break;
    }

    return *curr;
}

void AST::CheckUnaryArg(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("ERROR, not enough arguments\n");
    }

    if (func->next->type != TokenType::CLOSE_PARENT) {
        std::cerr << static_cast<int>(func->next->type) << std::endl;
        throw std::runtime_error("ERROR, too many arguments\n");
    }
}

int64_t AST::Add(std::shared_ptr<Pair> curr) {
    int64_t res = 0;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        if         (curr->type == TokenType::NUM) {
            res += (curr->value).TakeValue<int64_t>();
        } else if (curr->type == TokenType::OPEN_PARENT) {
            auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
            res += evaluated.value.TakeValue<int64_t>();
        } else {
            // ERROR, unexpected lexema in Add met
        }
    }

    return res;
}

int64_t AST::Sub(std::shared_ptr<Pair> curr) {
    if (!(curr = curr->next)) {
        // ERROR, absence of the next element
        return 0;
    }
    
    int64_t res = 0;
    if         (curr->type == TokenType::NUM) {
        res += (curr->value).TakeValue<int64_t>();
    } else if (curr->type == TokenType::OPEN_PARENT) {
        auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
        res = evaluated.value.TakeValue<int64_t>();
    } else {
        // ERROR, unexpected lexema in Sub met
    }  

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        if         (curr->type == TokenType::NUM) {
            res -= (curr->value).TakeValue<int64_t>();
        } else if (curr->type == TokenType::OPEN_PARENT) {
            auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
            res -= evaluated.value.TakeValue<int64_t>();
        } else {
            // ERROR, unexpected lexema in Sub met
        }        
    }

    return res;
}

int64_t AST::Mul(std::shared_ptr<Pair> curr) {
    int64_t res = 1;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        if         (curr->type == TokenType::NUM) {
            res *= (curr->value).TakeValue<int64_t>();
        } else if (curr->type == TokenType::OPEN_PARENT) {
            auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
            res *= evaluated.value.TakeValue<int64_t>();
        } else {
            // ERROR, unexpected lexema in Mul met
        }
    }

    return res;
}

int64_t AST::Div(std::shared_ptr<Pair> curr) {
    if (!(curr = curr->next)) {
        // ERROR, absence of the next element
        return 0;
    }
    
    int64_t res = 1; // due to no error handling
    if        (curr->type == TokenType::NUM) {
        res = (curr->value).TakeValue<int64_t>();
    } else if (curr->type == TokenType::OPEN_PARENT) {
        auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
        res = evaluated.value.TakeValue<int64_t>();
    } else {
        // ERROR, unexpected lexema in Div met
    }

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        if        (curr->type == TokenType::NUM) {
            res /= (curr->value).TakeValue<int64_t>();
        } else if (curr->type == TokenType::OPEN_PARENT) {
            auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
            res /= evaluated.value.TakeValue<int64_t>();
        } else {
            // ERROR, unexpected lexema in Div met
        }
    }

    return res;
}

int64_t AST::Abs(std::shared_ptr<Pair> curr) {
    CheckUnaryArg(curr);

    curr = curr->next;

    if (curr->type != TokenType::NUM) {
        curr->value = Evaluate(curr).value.TakeValue<int64_t>();
    }

    auto value = curr->value.TakeValue<int64_t>();

    return (value) > 0 ? value : -value;
}

int64_t AST::Min(std::shared_ptr<Pair> curr) {
    int64_t res = INT64_MAX;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        if        (curr->type == TokenType::NUM) {
            res = std::min(res, (curr->value).TakeValue<int64_t>());
        } else if (curr->type == TokenType::OPEN_PARENT) {
            auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
            res = std::min(res, evaluated.value.TakeValue<int64_t>());
        } else {
            // ERROR, unexpected lexema in Min met
        }
    }

    return res;
}

int64_t AST::Max(std::shared_ptr<Pair> curr) {
    int64_t res = INT64_MIN;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        if        (curr->type == TokenType::NUM) {
            res = std::max(res, (curr->value).TakeValue<int64_t>());
        } else if (curr->type == TokenType::OPEN_PARENT) {
            auto evaluated = Evaluate(curr->value.TakeValue<std::shared_ptr<Pair>>());
            res = std::max(res, evaluated.value.TakeValue<int64_t>());
        } else {
            // ERROR, unexpected lexema in Max met
        }
    }

    return res;
}

bool AST::EQ(std::shared_ptr<Pair> curr) {

}

bool AST::GT(std::shared_ptr<Pair> curr) {

}

bool AST::LT(std::shared_ptr<Pair> curr) {

}

bool AST::GEQ(std::shared_ptr<Pair> curr) {

}

bool AST::LEQ(std::shared_ptr<Pair> curr) {

}

bool AST::is_null(std::shared_ptr<AST::Pair> curr) {
    CheckUnaryArg(curr);
    curr = curr->next;
    /* Not implemented */
}

bool AST::is_pair(std::shared_ptr<Pair> curr) {
    CheckUnaryArg(curr);
    curr = curr->next;
    /* Not implemented */
}

bool AST::is_number(std::shared_ptr<Pair> curr) {
    CheckUnaryArg(curr);

    curr = curr->next;

    if (curr->type == TokenType::OPEN_PARENT) {
        auto evaluated = Evaluate(curr);
        curr->value = evaluated.value;
        curr->type = evaluated.type;
    }

    return (curr->value.TakeValue<Pair>().type == TokenType::NUM);
}

bool AST::is_bool(std::shared_ptr<Pair> curr) {
    CheckUnaryArg(curr);

    curr = curr->next;

    if (curr->type == TokenType::OPEN_PARENT) {
        curr->value = Evaluate(curr);
    }

    return (curr->value.TakeValue<Pair>().type == TokenType::BOOL);
}

bool AST::is_symb(std::shared_ptr<Pair> curr) {
    curr = curr->next;
    CheckUnaryArg(curr);
    /* Not implemented */
}

bool AST::is_list(std::shared_ptr<Pair> curr) {
    curr = curr->next;
    CheckUnaryArg(curr);
    /* Not implemented */
}
