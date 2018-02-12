#include "lisp.h"

void Evaluate::CheckOneArg(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("ERROR: Not enough arguments, expected 1.\n");
    }

    if (func->next->type != TokenType::CLOSE_PARENT) {
        std::cerr << static_cast<int>(func->next->type) << std::endl;
        throw std::runtime_error("ERROR: Too many arguments, expected 1.\n");
    }
}

void Evaluate::CheckAtLeastOneArg(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("ERROR: Not enough arguments, expected at least 1.\n");
    }
}

void Evaluate::CheckTwoArgs(std::shared_ptr<Pair> func) {
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

void Evaluate::CheckAtLeastTwoArgs(std::shared_ptr<Pair> func) {
    if (!(func = func->next)) {
        throw std::runtime_error("ERROR: Not enough arguments, expected at least 2 but got 0.\n");
    }

    if (!func->next) {
        throw std::runtime_error("ERROR: Not enough arguments, expected at least 2 but got 1.\n");
    }
}

int64_t Evaluate::Add(std::shared_ptr<Pair> curr) {
    int64_t res = 0;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        res += curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t Evaluate::Sub(std::shared_ptr<Pair> curr) {
    CheckAtLeastOneArg(curr);

    curr = curr->next;

    Eval(curr);
    auto res = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        res -= curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t Evaluate::Mul(std::shared_ptr<Pair> curr) {
    int64_t res = 1;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        res *= curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t Evaluate::Div(std::shared_ptr<Pair> curr) {
    CheckAtLeastOneArg(curr);

    curr = curr->next;

    Eval(curr);
    auto res = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        res /= curr->value.TakeValue<int64_t>();
    }

    return res;
}

int64_t Evaluate::Abs(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);

    curr = curr->next;

    Eval(curr);
    auto value = curr->value.TakeValue<int64_t>();

    return (value) > 0 ? value : -value;
}

int64_t Evaluate::Min(std::shared_ptr<Pair> curr) {
    int64_t res = INT64_MAX;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        res = std::min(res, curr->value.TakeValue<int64_t>());
    }

    return res;
}

int64_t Evaluate::Max(std::shared_ptr<Pair> curr) {
    int64_t res = INT64_MIN;
    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        res = std::max(res, curr->value.TakeValue<int64_t>());
    }

    return res;
}

bool Evaluate::EQ(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Eval(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        if (first != curr->value.TakeValue<int64_t>()) {
            return false;
        }
    }

    return true;
}

bool Evaluate::GT(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Eval(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first <= second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool Evaluate::LT(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Eval(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first >= second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool Evaluate::GEQ(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Eval(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first < second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool Evaluate::LEQ(std::shared_ptr<Pair> curr) {
    CheckAtLeastTwoArgs(curr);

    curr = curr->next;

    Eval(curr);
    auto first = curr->value.TakeValue<int64_t>();

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        auto second = curr->value.TakeValue<int64_t>();

        if (first > second) {
            return false;
        }

        first = second;
    }

    return true;
}

bool Evaluate::is_null(std::shared_ptr<Evaluate::Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Eval(curr);

    /* Not implemented */
}

bool Evaluate::is_pair(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Eval(curr);

    /* Not implemented */
}

bool Evaluate::is_number(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Eval(curr);

    return (curr->type == TokenType::NUM);
}

bool Evaluate::is_bool(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Eval(curr);

    return (curr->type == TokenType::BOOL);
}

bool Evaluate::is_symb(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Eval(curr);

    /* Not implemented */
}

bool Evaluate::is_list(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Eval(curr);

    /* Not implemented */
}

void Evaluate::If(std::shared_ptr<Pair> curr) {
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
    Eval(condition);

    if(condition->type == TokenType::BOOL &&
       (condition->value).TakeValue<bool>() == false) {
        if (no_else_branch) {
            throw std::runtime_error("ERROR: No else part to execute\n");
            return;
        }
        Eval(false_branch);
        curr->type = false_branch->type;
        curr->value = false_branch->value;
    } else {
        Eval(true_branch);
        curr->type = true_branch->type;
        curr->value = true_branch->value;
    }
}

bool Evaluate::NOT(std::shared_ptr<Pair> curr) {
    CheckOneArg(curr);
    curr = curr->next;
    Eval(curr);

    return (curr->type == TokenType::BOOL && curr->value.TakeValue<bool>() == false);
}

bool Evaluate::AND(std::shared_ptr<Pair> curr) {
    bool is_true = true;

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        if (curr->type == TokenType::BOOL && curr->value.TakeValue<bool>() == false) {
            is_true = false;
        }
    }

    return is_true;
}

bool Evaluate::OR(std::shared_ptr<Pair> curr) {
    bool is_true = false;

    while ((curr = curr->next)->type != TokenType::CLOSE_PARENT) {
        Eval(curr);
        if (curr->type != TokenType::BOOL || curr->value.TakeValue<bool>() != false) {
            is_true = true;
        }
    }

    return is_true;
}

bool Evaluate::ARE_EQ(std::shared_ptr<Pair> curr) {

}

bool Evaluate::ARE_EQUAL(std::shared_ptr<Pair> curr) {
    CheckTwoArgs(curr);
    curr = curr->next;
    Eval(curr);
    Eval(curr->next);

    if (curr->type != curr->next->type) {
        return false;
    }

    switch (curr->type) {
        case TokenType::NUM: {
            auto first = curr->value.TakeValue<int64_t>();
            auto second = curr->next->value.TakeValue<int64_t>();
            return first == second;
        }
        case TokenType::BOOL: {
            auto first = curr->value.TakeValue<bool>();
            auto second = curr->next->value.TakeValue<bool>();
            return first == second;
        }
        default:
            std::runtime_error("Incomparable types\n");
            break;
    }

}

bool Evaluate::INT_EQ(std::shared_ptr<Pair> curr) {

}
