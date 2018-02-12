#include <sstream>
#include <tic.h>
#include "lisp.h"

Tokenizer::Tokenizer(std::unique_ptr<std::istream> input_stream)
        : input_stream_(std::move(input_stream)) {}

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

AST::AST(std::unique_ptr<std::istream> input_stream)
        : Tokenizer(std::move(input_stream))
        , root_(std::make_shared<Pair>())
        , curr_(root_) {}

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

Evaluate::Evaluate(const std::string& expr)
        : AST(std::make_unique<std::stringstream>(expr))
        , std::string() {

    while (this->InsertLexema()) {}

    auto evaluated = Eval(root_);
    switch (evaluated.type) {
        case Tokenizer::TokenType::NUM:
            this->append(std::to_string(evaluated.value.TakeValue<int64_t>()));
            break;
        case Tokenizer::TokenType::BOOL:
            this->append(((evaluated.value.TakeValue<bool>()) ? "#t" : "#f"));
            break;
        default:
            break;
    }
}

const Evaluate::Pair& Evaluate::Eval(std::shared_ptr<Pair> curr) {
    switch (curr->type) {
        case TokenType::OPEN_PARENT: {
            auto res = Eval(curr->value.TakeValue<std::shared_ptr<Pair>>());
            curr->value = res.value;
            curr->type = res.type;
            }
            break;
        case TokenType::BUILTIN:
            switch (Tokenizer::builtins_.at(curr->value.TakeValue<std::string>())) {
                    // Integer math
                case Builtins::ADD:
                    curr->value = Add(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::SUB:
                    curr->value = Sub(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::MUL:
                    curr->value = Mul(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::DIV:
                    curr->value = Div(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::EQ:
                    curr->value = EQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::GT:
                    curr->value = GT(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::LT:
                    curr->value = LT(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::GEQ:
                    curr->value = GEQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::LEQ:
                    curr->value = LEQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::MIN:
                    curr->value = Min(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::MAX:
                    curr->value = Max(curr);
                    curr->type = TokenType::NUM;
                    break;
                case Builtins::ABS:
                    curr->value = Abs(curr);
                    curr->type = TokenType::NUM;
                    break;

                    // Predicates
                case Builtins::IS_NULL:
                    curr->value = is_null(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_PAIR:
                    curr->value = is_pair(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_NUMBER:
                    curr->value = is_number(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_BOOLEAN:
                    curr->value = is_bool(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_LIST:
                    curr->value = is_list(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::IS_SYMBOL:
                    curr->value = is_symb(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::ARE_EQUAL:
                    curr->value = ARE_EQUAL(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::ARE_EQ:
                    curr->value = ARE_EQ(curr);
                    curr->type = TokenType::BOOL;
                    break;
                case Builtins::INT_EQ:
                    curr->value = INT_EQ(curr);
                    curr->type = TokenType::BOOL;
                    break;

                    // Logic
                case Builtins::IF:
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
