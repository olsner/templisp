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
    //whitespace
    TEST("nil", " ()"_lisp);
    TEST("nil", " ( )"_lisp);
    TEST("nil", " \t\n( )"_lisp);
    TEST("nil", " \t\n( \t\n)"_lisp);
    TEST("symbol-after-newline", R"(
    symbol-after-newline )"_lisp);
    TEST("(list-after-newline)", R"(
    (list-after-newline) )"_lisp);
    TEST("(foo bar)", R"( (foo
    bar) )"_lisp);
    // comments
    TEST("nil", ";\n()"_lisp);
    TEST("nil", "; comment\n()"_lisp);
    TEST("nil", "(; comment\n)"_lisp);

    // strings
    TEST("string", "\"string\""_lisp); // TODO Change string printing to include quotes
}
