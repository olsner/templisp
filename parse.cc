#include <array>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string_view>

#include "types.h"

using namespace std;

template<typename ITEM> struct quote {};

// TODO do constexpr type-level printing too
template<typename CAR> void out_list(ostream& os, cons<CAR, nil> list) {
    os << CAR();
}
template<typename CAR, typename CADR, typename CDR> void out_list(ostream& os, cons<CAR, cons<CADR, CDR>> list) {
    os << CAR() << " ";
    out_list(os, cons<CADR, CDR>());
}
template<typename CAR, typename CDR> void out_list(ostream& os, cons<CAR, CDR> list) {
    os << CAR() << " . " << CDR();
}

ostream& operator<<(ostream& os, nil) {
    return os << "()";
}
template<typename ITEM> ostream& operator<<(ostream& os, quote<ITEM>) {
    return os << "(quote " << ITEM() << ")";
}
template<typename CAR, typename CDR> ostream& operator<<(ostream& os, cons<CAR, CDR> list) {
    out_list(os << "(", list);
    return os << ")";
}
template<char... STR> ostream& operator<<(ostream& os, symbol<STR...>) {
    return os << array { STR..., '\0' }.data();
}
template<typename T, T val> ostream& operator<<(ostream& os, value_type<T, val>) {
    return os << val;
}

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
    return is_member(c, "+-/*?<>!=") || is_letter(c) || (!initial && is_digit(c));
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
    using type = quote<typename p::type>;
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
    s << T();
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
        std::cout << "parsed: " << expr << std::endl; \
        compare_stringstream(expected, expr); \
    }

    TEST("()", "()"_lisp);
    TEST("()", " ()"_lisp);
    TEST("()", " ( )"_lisp);
    //TEST("()", " \t\n( \t\n)"_lisp);
    TEST("(quote ())", "'()"_lisp);
    TEST("(() ())", "(() ())"_lisp);
    TEST("(() () ())", "(() () ())"_lisp);
    // numbers
    TEST("12345", "12345"_lisp);
    TEST("(1 2 3 4 5)", "(1 2 3 4 5)"_lisp);
    // symbols
    TEST("foo", "foo"_lisp);
    TEST("(foo bar)", "(foo bar)"_lisp);
    TEST("(foo-bar-123+-/*?<>!= baz)", "(foo-bar-123+-/*?<>!= baz)"_lisp);
}
