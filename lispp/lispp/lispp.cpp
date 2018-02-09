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
        name_ = token;
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

Tokenizer::TokenType Tokenizer::ShowTokenType() const{
    return type_;
}

const std::string &Tokenizer::GetTokenName() const{
    return name_;
}

int64_t Tokenizer::GetTokenNumber() const {
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
            curr_->value = bools_.at(GetTokenName());
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
                    // Integer math
                case Builtins::ADD: // '+'
                    curr->value = Add(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::SUB: // '-'
                    curr->value = Sub(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::MUL: // '*'
                    curr->value = Mul(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::DIV: // '/'
                    curr->value = Div(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::EQ: // '='
                    curr->value = EQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::GT: // '>'
                    curr->value = GT(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::LT: // '<'
                    curr->value = LT(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::GEQ: // '>='
                    curr->value = GEQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::LEQ: // '<='
                    curr->value = LEQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::MIN: // 'min'
                    curr->value = Min(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::MAX: // 'max'
                    curr->value = Max(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::ABS: // 'abs'
                    curr->value = Abs(curr);
                    curr->type = TokenType::NUM;
                    break;

                    // Predicates
                case Builtins::IS_NULL: // 'null?'
                    curr->value = is_null(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_PAIR: // 'pair?'
                    curr->value = is_pair(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_NUMBER: // 'number?'
                    curr->value = is_number(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_BOOLEAN: // 'boolean?'
                    curr->value = is_bool(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_LIST: // 'list?'
                    curr->value = is_list(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_SYMBOL: // 'symb?'
                    curr->value = is_symb(curr);
                    curr->type = TokenType::BOOL;
                    break;

                    // Logic
                case Builtins::IF: //if
                    If(curr);
                    break;

                case Builtins::NOT:
                    curr->value = NOT(curr);
                    curr->type = TokenType::BOOL;
                    break;

                case Builtins::AND:
                    curr->value = AND(curr);
                    curr->type = TokenType::BOOL;
                    break;

                case Builtins::OR:
                    curr->value = OR(curr);
                    curr->type = TokenType::BOOL;
                    break;

                default:
                    break;
            }
            break;
        default:
            break;
    }

    return *curr;
}

void AST::CheckOneArg(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("ERROR: Not enough arguments, expected 1.\n");
    }

    if (func->next->type != TokenType::CLOSE_PARENT) {
        std::cerr << static_cast<int>(func->next->type) << std::endl;
        throw std::runtime_error("ERROR: Too many arguments, expected 1.\n");
    }
}

void AST::CheckAtLeastOneArg(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("ERROR: Not enough arguments, expected at least 1.\n");
    }
}

void AST::CheckTwoArgs(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("Not enough arguments, expected 2 but got 0.\n");
    }

    if (!(func = func->next)) {
        throw std::runtime_error("Not enough arguments, expected 2 but got 1.\n");
    }

    if (func->next->type != TokenType::CLOSE_PARENT) {
        std::cerr << static_cast<int>(func->next->type) << std::endl;
        throw std::runtime_error("ERROR: Too many arguments, expected 2.\n");
    }
}

void AST::CheckAtLeastTwoArgs(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("ERROR: Not enough arguments, expected at least 2 but got 0.\n");
    }

    if (!func->next) {
        throw std::runtime_error("ERROR: Not enough arguments, expected at least 2 but got 1.\n");
    }
}

int64_t AST::Add(std::shared_ptr<Pair> curr) {
    int64_t res = 0;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        res += curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t AST::Sub(std::shared_ptr<Pair> curr) {
    CheckAtLeastOneArg(curr);

    curr = curr->next;

    Evaluate(curr);
    auto res = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        res -= curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t AST::Mul(std::shared_ptr<Pair> curr) {
    int64_t res = 1;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        res *= curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t AST::Div(std::shared_ptr<Pair> curr) {
    CheckAtLeastOneArg(curr);

    curr = curr->next;

    Evaluate(curr);
    auto res = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        res /= curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t AST::Abs(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);

    curr = curr->next;

    Evaluate(curr);
    auto value = curr->value.TakeValue<int64_t>();

    return (value) > 0 ? value : -value;
}

int64_t AST::Min(std::shared_ptr<Pair> curr) {
    int64_t res = INT64_MAX;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        res = std::min(res, curr->value.TakeValue<int64_t>());
    }

    return res;
}

int64_t AST::Max(std::shared_ptr<Pair> curr) {
    int64_t res = INT64_MIN;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        res = std::max(res, curr->value.TakeValue<int64_t>());
    }

    return res;
}

bool AST::EQ(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Evaluate(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        if (first != curr->value.TakeValue<int64_t>()) {
            return false;
        }
    }

    return true;
}

bool AST::GT(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Evaluate(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first <= second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool AST::LT(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Evaluate(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first >= second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool AST::GEQ(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Evaluate(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first < second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool AST::LEQ(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;
    
    Evaluate(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first > second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool AST::is_null(std::shared_ptr<AST::Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Evaluate(curr);

    /* Not implemented */
}

bool AST::is_pair(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Evaluate(curr);

    /* Not implemented */
}

bool AST::is_number(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Evaluate(curr);

    return (curr->type == TokenType::NUM);
}

bool AST::is_bool(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Evaluate(curr);

    return (curr->type == TokenType::BOOL);
}

bool AST::is_symb(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Evaluate(curr);

    /* Not implemented */
}

bool AST::is_list(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Evaluate(curr);

    /* Not implemented */
}

void AST::If(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);
    bool no_else_branch = true;
    /*Check if 3rd of 4th token in sequence is close parent*/
    if (curr->next->next->next && 
        curr->next->next->next->type != TokenType::CLOSE_PARENT) {
        if(curr->next->next->next->next && 
           curr->next->next->next->next->type != TokenType::CLOSE_PARENT) {
            throw std::runtime_error("ERROR: Too many arguments, expected 2 or 3.\n");
            return;
        } else {
            no_else_branch = false;
        }
    }
    /*if((curr->next->next->next)->type != TokenType::CLOSE_PARENT ||
       (curr->next->next->next->next)->type != TokenType::CLOSE_PARENT) {
        throw std::runtime_error("ERROR: Too many arguments, expected 2 or 3.\n");        
    }*/

    auto condition = curr->next;
    auto true_branch = condition->next;
    auto false_branch = true_branch->next;
    Evaluate(condition);

    if(condition->type == TokenType::BOOL && 
      (condition->value).TakeValue<bool>() == false) {
        if (no_else_branch) {
            throw std::runtime_error("ERROR: No else part to execute\n");
            return;
        }
        Evaluate(false_branch);
        curr->type = false_branch->type;
        curr->value = false_branch->value;
    } else {
        Evaluate(true_branch);
        curr->type = true_branch->type;
        curr->value = true_branch->value;
    }
}

bool AST::NOT(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Evaluate(curr);

    return (curr->type == TokenType::BOOL && curr->value.TakeValue<bool>() == false);
}

bool AST::AND(std::shared_ptr<Pair> curr) {
    bool is_true = true;

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        if (curr->type == TokenType::BOOL && curr->value.TakeValue<bool>() == false) {
            is_true = false;
        }
    }

    return is_true;
}

bool AST::OR(std::shared_ptr<Pair> curr) {
    bool is_true = false;

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Evaluate(curr);
        if (curr->type != TokenType::BOOL || curr->value.TakeValue<bool>() != false) {
            is_true = true;
        }
    }

    return is_true;
}
