#include <iostream>
#include <sstream>
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
            return ((result.value.TakeValue<bool>()) ? "#t" : "#f");
        default:
            break;
    }
}

void ExpectEq(const std::string &expr, const std::string &ans) {
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
    ExpectEq("#f", "#f");
    ExpectEq("10", "10");

    /* Int math */
    ExpectEq("(+ 1 2)", "3");
    ExpectEq("(+ 1)", "1");
    ExpectEq("(+ 1 (+ 3 4 5))", "13");
    ExpectEq("(- 1 2)", "-1");
    ExpectEq("(- 2 1)", "1");
    ExpectEq("(* 5 6)", "30");
    ExpectEq("(/ 4 2)", "2");
    ExpectEq("(/ 4 2 2)", "1");
    ExpectEq("(+ (* 1 2 3) (- 2 3) (/ 4 2))", "7");
    ExpectEq("(+ 800 (- 100 230 (* 21 31 (/ 10 (- 3 2) 10))))", "19");

    ExpectEq("(+)", "0");
    ExpectEq("(*)", "1");

    ExpectEq("(abs 10)", "10");
    ExpectEq("(abs -10)", "10");
    ExpectEq("(abs -100)", "100");
    ExpectEq("(abs 241)", "241");
    ExpectEq("(abs (- 120 (- 423 123)))", "180");

    ExpectEq("(max 0)", "0");
    ExpectEq("(max 1 2)", "2");
    ExpectEq("(max 1 2 3 4 5)", "5");

    ExpectEq("(min 0)", "0");
    ExpectEq("(min 1 2)", "1");
    ExpectEq("(min 1 2 3 4 5)", "1");

    ExpectEq("(= 1 1)", "#t");
    ExpectEq("(= 1 4)", "#f");
    ExpectEq("(= (+ 10 5) (* 3 5))", "#t");
    ExpectEq("(= (min 12 292 -7) (max -254 -23 -7))", "#t");
    
    ExpectEq("(> 3 2 1)", "#t");
    ExpectEq("(> 3 63 0)", "#f");
    ExpectEq("(> 3 (min 3 4 2) 1)", "#t");

    ExpectEq("(>= 3 2 1)", "#t");
    ExpectEq("(>= 3 3 1)", "#t");
    ExpectEq("(>= 3 11 1)", "#f");

    ExpectEq("(< 1 2 3)", "#t");
    ExpectEq("(< 1 1 3)", "#f");

    ExpectEq("(<= 1 2 3)", "#t");
    ExpectEq("(<= 1 1 3)", "#t");
    ExpectEq("(<= 1 0 3)", "#f");

    
    /* Predicates */

    ExpectEq("(boolean? #t)", "#t");
    ExpectEq("(boolean? #f)", "#t");

    ExpectEq("(boolean? 1)", "#f");
    ExpectEq("(boolean? -42)", "#f");

    ExpectEq("(boolean? (+ 2 3))", "#f");
    ExpectEq("(boolean? (/ 4 2 2))", "#f");
    ExpectEq("(boolean? (+ 800 (- 100 230 (* 21 31 (/ 10 (- 3 2) 10)))))", "#f");
/*
    Test bool

    ExpectEq("#t", "#t");
    ExpectEq("#f", "#f");

    ExpectEq("(boolean? #t)", "#t");
    ExpectEq("(boolean? #f)", "#t");
    ExpectEq("(boolean? 1)", "#f");
    ExpectEq("(boolean? '())", "#f");

    ExpectEq("(not #f)", "#t");
    ExpectEq("(not #t)", "#f");
    ExpectEq("(not 1)", "#f");
    ExpectEq("(not 0)", "#f");
    ExpectEq("(not '())", "#f");

    ExpectRuntimeError("(not)");
    ExpectRuntimeError("(not #t #t)");

    ExpectEq("(and)", "#t");
    ExpectEq("(and (= 2 2) (> 2 1))", "#t");
    ExpectEq("(and (= 2 2) (< 2 1))", "#f");
    ExpectEq("(and 1 2 'c '(f g))", "(f g)");

    ExpectNoError("(define x 1)");
    ExpectNoError("(and #f (set! x 2))");
    ExpectEq("x", "1");

    ExpectEq("(or)", "#f");
    ExpectEq("(or (not (= 2 2)) (> 2 1))", "#t");
    ExpectEq("(or #f (< 2 1))", "#f");
    ExpectEq("(or #f 1)", "1");

    ExpectNoError("(define x 1)");
    ExpectNoError("(or #t (set! x 2))");
    ExpectEq("x", "1");




    Test control flow

    ExpectEq("(if #t 0)", "0");
    ExpectEq("(if #f 0)", "()");
    ExpectEq("(if (= 2 2) (+ 1 10))", "11");
    ExpectEq("(if (= 2 3) (+ 1 10) 5)", "5");

    ExpectNoError("(define x 1)");

    ExpectNoError("(if #f (set! x 2))");
    ExpectEq("x", "1");

    ExpectNoError("(if #t (set! x 4) (set! x 3))");
    ExpectEq("x", "4");

    ExpectSyntaxError("(if)");
    ExpectSyntaxError("(if 1 2 3 4)");



    Test eval

    ExpectEq("(quote (1 2))", "(1 2)");


    Test int

    ExpectEq("4", "4");
    ExpectEq("-14", "-14");
    ExpectEq("+14", "14");

    ExpectEq("(number? -1)", "#t");
    ExpectEq("(number? 1)", "#t");
    ExpectEq("(number? #t)", "#f");

    ExpectEq("(=)", "#t");
    ExpectEq("(>)", "#t");
    ExpectEq("(<)", "#t");
    ExpectEq("(>=)", "#t");
    ExpectEq("(<=)", "#t");

    ExpectEq("(= 1 2)", "#f");
    ExpectEq("(= 1 1)", "#t");
    ExpectEq("(= 1 1 1)", "#t");
    ExpectEq("(= 1 1 2)", "#f");

    ExpectEq("(> 2 1)", "#t");
    ExpectEq("(> 1 1)", "#f");
    ExpectEq("(> 3 2 1)", "#t");
    ExpectEq("(> 3 2 3)", "#f");

    ExpectEq("(< 1 2)", "#t");
    ExpectEq("(< 1 1)", "#f");
    ExpectEq("(< 1 2 3)", "#t");
    ExpectEq("(< 1 2 1)", "#f");

    ExpectEq("(>= 2 1)", "#t");
    ExpectEq("(>= 1 2)", "#f");
    ExpectEq("(>= 3 3 2)", "#t");
    ExpectEq("(>= 3 3 4)", "#f");

    ExpectEq("(<= 2 1)", "#f");
    ExpectEq("(<= 1 2)", "#t");
    ExpectEq("(<= 3 3 4)", "#t");
    ExpectEq("(<= 3 3 2)", "#f");

    ExpectRuntimeError("(= 1 #t)");
    ExpectRuntimeError("(< 1 #t)");
    ExpectRuntimeError("(> 1 #t)");
    ExpectRuntimeError("(<= 1 #t)");
    ExpectRuntimeError("(>= 1 #t)");

    ExpectRuntimeError("(max)");
    ExpectRuntimeError("(min)");

    ExpectRuntimeError("(max #t)");
    ExpectRuntimeError("(min #t)");

    ExpectRuntimeError("(/)");
    ExpectRuntimeError("(-)");
    ExpectRuntimeError("(+ 1 #t)");
    ExpectRuntimeError("(- 1 #t)");
    ExpectRuntimeError("(* 1 #t)");
    ExpectRuntimeError("(/ 1 #t)");

    ExpectRuntimeError("(abs)");
    ExpectRuntimeError("(abs #t)");
    ExpectRuntimeError("(abs 1 2)");



    Test lambdas

    ExpectEq("((lambda (x) (+ 1 x)) 5)", "6");

    ExpectNoError("(define test (lambda (x) (set! x (* x 2)) (+ 1 x)))");
    ExpectEq("(test 20)", "41");

    ExpectNoError("(define slow-add (lambda (x y) (if (= x 0) y (slow-add (- x 1) (+ y 1)))))");
    ExpectEq("(slow-add 3 3)", "6");
    ExpectEq("(slow-add 100 100)", "200");


    ExpectNoError("(define x 1)");

    ExpectNoError(R"(
        (define range
          (lambda (x)
            (lambda ()
              (set! x (+ x 1))
              x)))
                    )");

    ExpectNoError("(define my-range (range 10))");
    ExpectEq("(my-range)", "11");
    ExpectEq("(my-range)", "12");
    ExpectEq("(my-range)", "13");

    ExpectEq("x", "1");

    ExpectSyntaxError("(lambda)");
    ExpectSyntaxError("(lambda x)");
    ExpectSyntaxError("(lambda (x))");

    ExpectNoError("(define (inc x) (+ x 1))");
    ExpectEq("(inc -1)", "0");

    ExpectNoError("(define (add x y) (+ x y 1))");
    ExpectEq("(add -10 10)", "1");

    ExpectNoError("(define (zero) 0)");
    ExpectEq("(zero)", "0");


    Test list

    ExpectRuntimeError("()");
    ExpectRuntimeError("(1)");
    ExpectRuntimeError("(1 2 3)");

    ExpectEq("'()", "()");
    ExpectEq("'(1)", "(1)");
    ExpectEq("'(1 2)", "(1 2)");

    ExpectEq("'(1 . 2)", "(1 . 2)");
    ExpectSyntaxError("(1 . 2 3)");

    ExpectEq("'(1 2 . 3)", "(1 2 . 3)");
    ExpectEq("'(1 2 . ())", "(1 2)");
    ExpectEq("'(1 . (2 . ()))", "(1 2)");

    ExpectSyntaxError("((1)");
    ExpectSyntaxError("(1))");
    ExpectSyntaxError(")(1)");

    ExpectSyntaxError("(.)");
    ExpectSyntaxError("(1 .)");
    ExpectSyntaxError("(. 2)");

    ExpectEq("(pair? '(1 . 2))", "#t");
    ExpectEq("(pair? '(1 2))", "#t");
    ExpectEq("(pair? '())", "#f");

    ExpectEq("(null? '())", "#t");
    ExpectEq("(null? '(1 2))", "#f");
    ExpectEq("(null? '(1 . 2))", "#f");

    ExpectEq("(list? '())", "#t");
    ExpectEq("(list? '(1 2))", "#t");
    ExpectEq("(list? '(1 . 2))", "#f");
    ExpectEq("(list? '(1 2 3 4 . 5))", "#f");

    ExpectEq("(cons 1 2)", "(1 . 2)");
    ExpectEq("(car '(1 . 2))", "1");
    ExpectEq("(cdr '(1 . 2))", "2");

    ExpectNoError("(define x '(1 . 2))");

    ExpectNoError("(set-car! x 5)");
    ExpectEq("(car x)", "5");

    ExpectNoError("(set-cdr! x 6)");
    ExpectEq("(cdr x)", "6");

    ExpectEq("(list)", "()");
    ExpectEq("(list 1)", "(1)");
    ExpectEq("(list 1 2 3)", "(1 2 3)");

    ExpectEq("(list-ref '(1 2 3) 1)", "2");
    ExpectEq("(list-tail '(1 2 3) 1)", "(2 3)");
    ExpectEq("(list-tail '(1 2 3) 3)", "()");

    ExpectRuntimeError("(list-ref '(1 2 3) 3)");
    ExpectRuntimeError("(list-ref '(1 2 3) 10)");
    ExpectRuntimeError("(list-tail '(1 2 3) 10)");



    Test symbol


    ExpectNameError("x");

    ExpectEq("'x", "x");
    ExpectEq("(quote x)", "x");

    ExpectEq("(symbol? 'x)", "#t");
    ExpectEq("(symbol? 1)", "#f");

    ExpectNoError("(define x (+ 1 2))");
    ExpectEq("x", "3");

    ExpectNoError("(define x (+ 2 2))");
    ExpectEq("x", "4");

    ExpectSyntaxError("(define)");
    ExpectSyntaxError("(define 1)");
    ExpectSyntaxError("(define x 1 2)");

    ExpectNameError("(set! x 2)");
    ExpectNameError("x");

    ExpectNoError("(define x 1)");
    ExpectEq("x", "1");

    ExpectNoError("(set! x (+ 2 4))");
    ExpectEq("x", "6");

    ExpectSyntaxError("(set!)");
    ExpectSyntaxError("(set! 1)");
    ExpectSyntaxError("(set! x 1 2)");

*/
    return 0;
}
