#pragma once

#include <type_traits>

namespace {
namespace parser {

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
template<char... STR>
struct parse<string_holder<'-', STR...>> {
    using p = parse_num<string_holder<STR...>>;
    using type = value_type<typename p::type::type, -p::type::value>;
    using tail = typename p::tail;
};

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

}

template<char... STR> using parse_t = typename parser::parse_s<STR...>::type;
template<typename C, C... STR> constexpr auto operator"" _lisp() { return parse_t<STR...>(); }

}
