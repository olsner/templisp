#include <iostream>
#include <sstream>

#include "types.h"
#include "print.h"
#include "lists.h"
#include "parse.h"

using namespace std;

template<typename T> void compare_stringstream(const char* expected, T) {
    std::stringstream s;
    s << print<T>();
    if (s.str() != expected) {
        std::cout << "Test failed: Expected " << expected << " but got " << s.str() << std::endl;
        std::cout << "typeid: " << typeid(T).name() << std::endl; \
    }
}

int main()
{
    // Store to a variable to ensure constexprness
#define TEST(expected, lit) \
    { \
        constexpr auto expr = lit; \
        std::cout << "parsed: " << print(expr) << std::endl; \
        compare_stringstream(expected, expr); \
    }

    TEST("nil", "()"_lisp);
    TEST("nil", " ()"_lisp);
    TEST("nil", " ( )"_lisp);
    //TEST("()", " \t\n( \t\n)"_lisp);
    TEST("(quote nil)", "'()"_lisp);
    TEST("(nil nil)", "(() ())"_lisp);
    TEST("(nil nil nil)", "(() () ())"_lisp);
    // numbers
    TEST("12345", "12345"_lisp);
    TEST("-48", "-48"_lisp); // TODO Check that we get a number and not a symbol, they'll print the same...
    TEST("(1 2 3 4 5)", "(1 2 3 4 5)"_lisp);
    // symbols
    TEST("foo", "foo"_lisp);
    TEST("(foo bar)", "(foo bar)"_lisp);
    TEST("(foo-bar-123+-/*?<>!= baz)", "(foo-bar-123+-/*?<>!= baz)"_lisp);
}
