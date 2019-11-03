#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "types.h"
#include "print.h"
#include "lists.h"

using namespace std;

constexpr bool is_digit(char c) {
    return '0' <= c && c <= '9';
}
constexpr bool is_member(char c, string_view s) {
    for (char m : s) {
        if (m == c) return true;
    }
    return false;
}
constexpr bool is_letter(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
constexpr bool is_symchar(char c, bool initial = false) {
    return is_member(c, "+/*?<>!=") || is_letter(c) || (!initial && (is_digit(c) || c == '-'));
}

template<char... STR> struct string_holder;

template<typename T, typename Enable = void> struct parse;
template<char... STR> using parse_s = parse<string_holder<STR...>>;
template<char... STR> using parse_t = typename parse_s<STR...>::type;

template<char... STR> struct parse_open_list;
template<char... STR> using parse_open_list_s = parse_open_list<STR...>;
template<typename T> struct parse_list_tail;
template<char... STR> struct parse_list_tail<string_holder<STR...>>: parse_open_list<STR...> {};

template<typename S, int acc = 0, typename Enable = void> struct parse_num;
template<typename S, typename Sym = symbol<>, typename Enable = void> struct parse_sym;

#define FORWARD(...) \
    using p = __VA_ARGS__; \
    using type = typename p::type; \
    using tail = typename p::tail;

// TODO Match and strip more whitespace (add a way to do it with is_whitespace
// to avoid duplication between parse and parse_open_list)
template<char... STR> struct parse<string_holder<' ', STR...>> { FORWARD(parse_s<STR...>) };
template<char... STR> struct parse<string_holder<'(', STR...>> { FORWARD(parse_open_list_s<STR...>) };
template<char... STR> struct parse<string_holder<'\'', STR...>> {
    using p = parse_s<STR...>;
    using type = list_t<decltype("quote"_sym), typename p::type>;
    using tail = typename p::tail;
};
// template<char... STR> struct parse<'\"', STR...> // TODO Strings
template<char C, char... STR>
struct parse<string_holder<C, STR...>, enable_if_t<is_symchar(C, true)>> {
    FORWARD(parse_sym<string_holder<STR...>, symbol<C>>);
};

template<char... SYM, char C, char... STR>
struct parse_sym<string_holder<C, STR...>, symbol<SYM...>, enable_if_t<is_symchar(C)>>:
    parse_sym<string_holder<STR...>, symbol<SYM..., C>> {};
template<typename Sym, char C, char... STR>
struct parse_sym<string_holder<C, STR...>, Sym, enable_if_t<!is_symchar(C)>> {
    using type = Sym;
    using tail = string_holder<C, STR...>;
};
template<typename Sym> struct parse_sym<string_holder<>, Sym> {
    using type = Sym;
    using tail = string_holder<>;
};

template<char C, char... STR>
struct parse<string_holder<C, STR...>, enable_if_t<is_digit(C)>>: parse_num<string_holder<C, STR...>> {};

template<int acc, char C, char... STR>
struct parse_num<string_holder<C, STR...>, acc, enable_if_t<is_digit(C)>>:
    parse_num<string_holder<STR...>, acc * 10 + (C - '0')> {};
template<int acc, char C, char... STR>
struct parse_num<string_holder<C, STR...>, acc, enable_if_t<!is_digit(C)>> {
    using type = value_type<int, acc>;
    using tail = string_holder<C, STR...>;
};
template<int acc> struct parse_num<string_holder<>, acc> {
    using type = value_type<int, acc>;
    using tail = string_holder<>;
};

template<char... STR> struct parse_open_list<' ', STR...> { FORWARD(parse_open_list<STR...>) };
template<char... STR> struct parse_open_list<')', STR...> {
    using type = nil;
    using tail = string_holder<STR...>;
};

// TODO Dotted list

template<char... STR> struct parse_open_list {
    using car = parse<string_holder<STR...>>;
    using cdr = parse_list_tail<typename car::tail>;
    using type = cons<typename car::type, typename cdr::type>;
    using tail = typename cdr::tail;
};

template<typename C, C... STR> constexpr auto operator"" _lisp() { return parse_t<STR...>(); }

template<typename T> void compare_stringstream(const char* expected, T) {
    std::stringstream s;
    s << print<T>();
    if (s.str() != expected) {
        std::cout << "Test failed: Expected " << expected << " but got " << s.str() << std::endl;
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
