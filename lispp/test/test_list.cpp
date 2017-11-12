#include "lisp_test.h"

TEST_CASE_METHOD(LispTest, "ListsAreNotSelfEvaliating") {
    ExpectRuntimeError("()");
    ExpectRuntimeError("(1)");
    ExpectRuntimeError("(1 2 3)");

    ExpectEq("'()", "()");
    ExpectEq("'(1)", "(1)");
    ExpectEq("'(1 2)", "(1 2)");
}

TEST_CASE_METHOD(LispTest, "ListSyntax") {
    ExpectEq("'(1 . 2)", "(1 . 2)");
    ExpectSyntaxError("(1 . 2 3)");

    ExpectEq("'(1 2 . 3)", "(1 2 . 3)");
    ExpectEq("'(1 2 . ())", "(1 2)");
    ExpectEq("'(1 . (2 . ()))", "(1 2)");
}

TEST_CASE_METHOD(LispTest, "ListInvalidSyntax") {
    ExpectSyntaxError("((1)");
    ExpectSyntaxError("(1))");
    ExpectSyntaxError(")(1)");

    ExpectSyntaxError("(.)");
    ExpectSyntaxError("(1 .)");
    ExpectSyntaxError("(. 2)");
}

TEST_CASE_METHOD(LispTest, "PairPredicate") {
    ExpectEq("(pair? '(1 . 2))", "#t");
    ExpectEq("(pair? '(1 2))", "#t");
    ExpectEq("(pair? '())", "#f");
}

TEST_CASE_METHOD(LispTest, "NullPredicate") {
    ExpectEq("(null? '())", "#t");
    ExpectEq("(null? '(1 2))", "#f");
    ExpectEq("(null? '(1 . 2))", "#f");
}

TEST_CASE_METHOD(LispTest, "ListPredicate") {
    ExpectEq("(list? '())", "#t");
    ExpectEq("(list? '(1 2))", "#t");
    ExpectEq("(list? '(1 . 2))", "#f");
    ExpectEq("(list? '(1 2 3 4 . 5))", "#f");
}

TEST_CASE_METHOD(LispTest, "PairOperations") {
    ExpectEq("(cons 1 2)", "(1 . 2)");
    ExpectEq("(car '(1 . 2))", "1");
    ExpectEq("(cdr '(1 . 2))", "2");
}

TEST_CASE_METHOD(LispTest, "PairMutations") {
    ExpectNoError("(define x '(1 . 2))");

    ExpectNoError("(set-car! x 5)");
    ExpectEq("(car x)", "5");

    ExpectNoError("(set-cdr! x 6)");
    ExpectEq("(cdr x)", "6");
}

TEST_CASE_METHOD(LispTest, "ListOperations") {
    ExpectEq("(list)", "()");
    ExpectEq("(list 1)", "(1)");
    ExpectEq("(list 1 2 3)", "(1 2 3)");

    ExpectEq("(list-ref '(1 2 3) 1)", "2");
    ExpectEq("(list-tail '(1 2 3) 1)", "(2 3)");
    ExpectEq("(list-tail '(1 2 3) 3)", "()");

    ExpectRuntimeError("(list-ref '(1 2 3) 3)");
    ExpectRuntimeError("(list-ref '(1 2 3) 10)");
    ExpectRuntimeError("(list-tail '(1 2 3) 10)");
}
