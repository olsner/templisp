#include <iostream>
#include <sstream>

#include "types.h"
#include "print.h"
#include "lists.h"
#include "parse.h"

using namespace std;

template<typename T, bool repr> void compare_stringstream(const char* expected) {
    // compile-time assert?
    if (strcmp(expected, printed<T, repr>)) {
        std::cout << "Failed: Expected " << expected << " but got " << printed<T> << std::endl;
    }
}

template<bool repr = false, typename T> void test(const char* expected, T = T()) {
    std::cout << "Parsed: " << printed<T, repr> << std::endl;
    compare_stringstream<T, repr>(expected);
}
template<typename T> void test_repr(const char* expected, T = T()) {
    test<true, T>(expected);
}

int main()
{
    test("nil", "()"_lisp);
    test("(quote nil)", "'()"_lisp);
    // lists
    test("(nil nil)", "(() ())"_lisp);
    test("(nil nil nil)", "(() () ())"_lisp);
    //test("(foo . bar)", "(foo . bar)"_lisp); // TODO No dotted pair parsing yet

    // numbers
    test("12345", "12345"_lisp);
    test("-48", "-48"_lisp);
    // Double-check that -48 becomes an integer and not a symbol starting with -
    static_assert(std::is_same_v<decltype("-48"_lisp), value_type<int, -48>>);
    test("(1 2 3 4 5)", "(1 2 3 4 5)"_lisp);

    // symbols
    test("foo", "foo"_lisp);
    test("(foo bar)", "(foo bar)"_lisp);
    test("(foo-bar-123+-/*?<>!= baz)", "(foo-bar-123+-/*?<>!= baz)"_lisp);

    //whitespace
    test("nil", " ()"_lisp);
    test("nil", " ( )"_lisp);
    test("nil", " \t\n( )"_lisp);
    test("nil", " \t\n( \t\n)"_lisp);
    test("symbol-after-newline", R"(
    symbol-after-newline )"_lisp);
    test("(list-after-newline)", R"(
    (list-after-newline) )"_lisp);
    test("(foo bar)", R"( (foo
    bar) )"_lisp);

    // comments
    test("nil", ";\n()"_lisp);
    test("nil", "; comment\n()"_lisp);
    test("nil", "(; comment\n)"_lisp);

    // strings
    test("string", "\"string\""_lisp);

    // Printing
    test("(foo . bar)", cons("foo"_sym, "bar"_sym)); // TODO No dotted pair parsing
    test_repr("\"string\"", "\"string\""_lisp);
    test_repr("(\"string\" bar)", "(\"string\" bar)"_lisp);
    //test_repr("'c'", "'c'"_lisp); // character literals don't exist in this LISP, those are just integers
}
