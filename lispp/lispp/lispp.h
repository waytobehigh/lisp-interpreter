#pragma once

#include <istream>
#include <string>
#include <unordered_set>
#include <vector>

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

    std::shared_ptr<Pair> value;
    std::shared_ptr<Pair> next;
};

class AST : public Tokenizer {
public:
    AST(std::istream* input_stream);
    void InsertLexema();

private:
    std::shared_ptr<Pair> curr_ = std::make_shared<Pair>();
    std::vector<std::shared_ptr<Pair> > return_stack_;
};

class Object {
    struct ObjectConcept {
        virtual ~ObjectConcept() {}
    };

    template< typename T > struct ObjectModel : ObjectConcept {
        ObjectModel( const T& t )
                : object( t ) {}
        
        virtual ~ObjectModel() {}

    private:
        T object;
    };

    std::shared_ptr<ObjectConcept> object;

public:
    template< typename T > Object( const T& obj )
            : object(new ObjectModel<T>(obj)) {}
};
