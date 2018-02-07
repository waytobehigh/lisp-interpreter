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
    const std::unordered_map<std::string, int> bools_ = {
            {"#f", 0}, // +
            {"#t", 1} // +
    };

    const std::unordered_map<std::string, int> builtins_ = {
            //  Special forms
            {"if", 1},
            {"quote", 2},
            {"lambda", 3},
            {"and", 4},
            {"or", 5},
            {"define", 6},
            {"set!", 7},

            //  Predicates
            {"null?", 8},
            {"pair?", 9},
            {"number?", 10},
            {"boolean?", 11},
            {"symbol?", 12},
            {"list?", 13},
            {"eq?", 14},
            {"equal?", 15},
            {"integer-equal?", 16},

            //  Logic
            {"not", 17},

            //  Integer math
            {"+", 20}, // +
            {"-", 21}, // +
            {"*", 22}, // +
            {"/", 23}, // +
            {"=", 24},
            {">", 25},
            {"<", 26},
            {">=", 27},
            {"<=", 28},
            {"min", 29},
            {"max", 30},
            {"abs", 31}, // +

            //  List functions
            {"cons", 32},
            {"car", 33},
            {"cdr", 34},
            {"set-car!", 35},
            {"set-cdr!", 36},
            {"list", 37},
            {"list-ref", 38},
            {"list-tail", 39},
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

    void CheckUnaryArg(std::shared_ptr<Pair> func);

    std::shared_ptr<Pair> curr_ = std::make_shared<Pair>();
    std::shared_ptr<Pair> root_ = curr_;
    std::vector<std::shared_ptr<Pair> > return_stack_;
};