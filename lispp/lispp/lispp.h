#pragma once

#include <istream>
#include <string>
#include <vector>
#include <memory>

#include <unordered_map>
#include <functional>

#include <climits>

#include "any.h"

class Tokenizer {
public:
    explicit Tokenizer(std::istream* input_stream);

    enum class TokenType {
        UNKNOWN, // 0
        NAME, // 1
        NUM, // 2
        BOOL, // 3
        BUILTIN, // 4
        OPEN_PARENT, // 5
        CLOSE_PARENT, // 6
        PAIR, // 7
        APOSTROPH, // 8
        END_OF_FILE // 9
    };

    enum class Builtins {
        // Special forms
        IF,
        QUOTE,
        LAMBDA,
        DEFINE,
        SET,

        // Predicates
        IS_NULL,
        IS_PAIR,
        IS_NUMBER,
        IS_BOOLEAN,
        IS_SYMBOL,
        IS_LIST,
        ARE_EQ,
        ARE_EQUAL,
        INTEGER_EQUAL,

        // Logic
        NOT,
        AND,
        OR,

        // Integer math
        ADD,
        SUB,
        MUL,
        DIV,
        EQ,
        GT,
        LT,
        GEQ,
        LEQ,
        MIN,
        MAX,
        ABS,

        // List functions
        CONS,
        CAR,
        CDR,
        SET_CAR,
        SET_CDR,
        LIST,
        LIST_REF,
        LIST_TAIL
    };

    void ReadNext();

    TokenType ShowTokenType();

    const std::string& GetTokenName();

    int64_t GetTokenNumber();

private:
    bool IsNumber(const std::string& token);
    bool IsName(const std::string& token);
    bool IsBool(const std::string &token);
    bool IsBuiltin(const std::string &token);

    std::istream* input_stream_;
    TokenType type_;
    std::string name_;
    int64_t number_;

protected:
    const std::unordered_map<std::string, bool> bools_ = {
            {"#f", true}, // +
            {"#t", false} // +
    };

    const std::unordered_map<std::string, Builtins> builtins_ = {
            // Special forms
            {"if", Builtins::IF},
            {"quote", Builtins::QUOTE},
            {"lambda", Builtins::LAMBDA},
            {"define", Builtins::DEFINE},
            {"set!", Builtins::SET},

            //  Predicates
            {"null?", Builtins::IS_NULL},
            {"pair?", Builtins::IS_PAIR},
            {"number?", Builtins::IS_NUMBER}, // +
            {"boolean?", Builtins::IS_BOOLEAN}, // +
            {"symbol?", Builtins::IS_SYMBOL},
            {"list?", Builtins::LIST},
            {"eq?", Builtins::ARE_EQ},
            {"equal?", Builtins::ARE_EQUAL},
            {"integer-equal?", Builtins::INTEGER_EQUAL},

            //  Logic
            {"not", Builtins::NOT},
            {"and", Builtins::AND},
            {"or", Builtins::OR},

            //  Integer math
            {"+", Builtins::ADD}, // +
            {"-", Builtins::SUB}, // +
            {"*", Builtins::MUL}, // +
            {"/", Builtins::DIV}, // +
            {"=", Builtins::EQ}, // +
            {">", Builtins::GT}, // +
            {"<", Builtins::LT}, // +
            {">=", Builtins::GEQ}, // +
            {"<=", Builtins::LEQ}, // +
            {"min", Builtins::MIN}, // +
            {"max", Builtins::MAX}, // +
            {"abs", Builtins::ABS}, // +

            //  List functions
            {"cons", Builtins::CONS},
            {"car", Builtins::CAR},
            {"cdr", Builtins::CDR},
            {"set-car!", Builtins::SET_CAR},
            {"set-cdr!", Builtins::SET_CDR},
            {"list", Builtins::LIST},
            {"list-ref", Builtins::LIST_REF},
            {"list-tail", Builtins::LIST_TAIL},
    };
};

class AST : protected Tokenizer {
public:
    struct Pair {
        Pair();

        Tokenizer::TokenType type;
        Any value;

        std::shared_ptr<Pair> next;
    };

    AST(std::istream* input_stream);
    std::shared_ptr<Pair> InsertLexema();
    const Pair& Evaluate(std::shared_ptr<Pair> curr);

private:
    inline void TurnNext();
    inline void TurnDown();
    void TEST_StatusDump();

    int64_t Add(std::shared_ptr<Pair> curr);
    int64_t Sub(std::shared_ptr<Pair> curr);
    int64_t Mul(std::shared_ptr<Pair> curr);
    int64_t Div(std::shared_ptr<Pair> curr);
    int64_t Abs(std::shared_ptr<Pair> curr);
    int64_t Min(std::shared_ptr<Pair> curr);
    int64_t Max(std::shared_ptr<Pair> curr);

    bool EQ(std::shared_ptr<Pair> curr);
    bool GT(std::shared_ptr<Pair> curr);
    bool LT(std::shared_ptr<Pair> curr);
    bool GEQ(std::shared_ptr<Pair> curr);
    bool LEQ(std::shared_ptr<Pair> curr);

    bool is_null(std::shared_ptr<Pair> curr);
    bool is_pair(std::shared_ptr<Pair> curr);
    bool is_number(std::shared_ptr<Pair> curr);
    bool is_bool(std::shared_ptr<Pair> curr);
    bool is_symb(std::shared_ptr<Pair> curr);
    bool is_list(std::shared_ptr<Pair> curr);

    void CheckOneArg(std::shared_ptr<Pair> func);
    void CheckAtLeastOneArg(std::shared_ptr<Pair> func);

    void CheckTwoArgs(std::shared_ptr<Pair> func);
    void CheckAtLeastTwoArgs(std::shared_ptr<Pair> func);

    std::shared_ptr<Pair> curr_ = std::make_shared<Pair>();
    std::shared_ptr<Pair> root_ = curr_;
    std::vector<std::shared_ptr<Pair> > return_stack_;
};
