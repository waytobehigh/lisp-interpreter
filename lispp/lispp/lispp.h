#pragma once

#include <istream>
#include <string>
#include <unordered_set>
#include <vector>
#include <memory>

#include <unordered_map>
#include <functional>

#include "any.h"


#define TEST__DUMP

class Tokenizer {
public:
    explicit Tokenizer(std::istream* input_stream);

    enum class TokenType {
        UNKNOWN,
        NAME,
        NUMBER,
        BUILTIN,
        OPEN_PARENT,
        CLOSE_PARENT,
        PAIR,
        APOSTROPH,
        END_OF_FILE
    };

    void ReadNext();

    TokenType ShowTokenType();

    const std::string& GetTokenName();

    int64_t GetTokenNumber();

private:
    bool IsNumber(const std::string& token);
    bool IsName(const std::string& token);
    bool IsBuiltin(const std::string &token);

    std::istream* input_stream_;
    TokenType type_;
    std::string name_;
    int64_t number_;
    const std::unordered_set<std::string> builtins_ = {
            //  Special forms
            "if",
            "quote",
            "lambda",
            "and",
            "or",
            "define",
            "set!",

            //  Predicates
            "null?"
            "pair?",
            "number?",
            "boolean?",
            "symbol?",
            "list?",
            "eq?",
            "equal?",
            "integer-equal?",

            //  Logic
            "not",
            "#t",
            "#f",

            //  Integer math
            "+",
            "-",
            "*",
            "/",
            "=",
            ">",
            "<",
            ">=",
            "<=",
            "min",
            "max",
            "abs",

            //  List functions
            "cons",
            "car",
            "cdr",
            "set-car!",
            "set-cdr!",
            "list",
            "list-ref",
            "list-tail",
    };
};

struct Pair {
    Pair();

    Tokenizer::TokenType type;
    Any value;

    std::shared_ptr<Pair> next;
};

class AST : public Tokenizer {
public:
    AST(std::istream* input_stream);
    void InsertLexema();
    int64_t Evaluate(std::shared_ptr<Pair> curr);

private:
    inline void TurnNext();
    inline void TurnDown();
    void TEST_StatusDump();

    int64_t Add(std::shared_ptr<Pair> curr);



    std::shared_ptr<Pair> curr_ = std::make_shared<Pair>();
    std::shared_ptr<Pair> root_ = curr_;
    std::vector<std::shared_ptr<Pair> > return_stack_;
};