#pragma once

#include <istream>
#include <string>
#include <unordered_set>

class Tokenizer {
public:
    explicit Tokenizer(std::istream* input_stream);

    enum class TokenType {
        NAME,
        NUMBER,
        OPEN_PARENT,
        CLOSE_PARENT,
        PAIR,
        SHARP,
        APOSTROPH,
        UNKNOWN,
        END_OF_FILE
    };

    void ReadNext();

    TokenType ShowTokenType();

    const std::string& GetTokenName();

    int64_t GetTokenNumber();

private:
    bool SetNumber(const std::string &token);
    bool IsName(const std::string& token);

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
            "null?",
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
