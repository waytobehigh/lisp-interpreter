#include "lisp_test.h"

TEST_CASE_METHOD(LispTest, "Quote") {
    ExpectEq("(quote (1 2))", "(1 2)");
}
