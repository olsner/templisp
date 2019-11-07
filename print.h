#pragma once

#include <cstring>
#include <iostream>

namespace {

template<typename T, bool term = true> struct to_data;
template<char... STR> struct to_data<string<STR...>, true> {
    static constexpr char array[] = { STR..., '\0' };
};
template<char... STR> struct to_data<string<STR...>, false> {
    static constexpr char array[] = { STR... };
};

template<typename... T> struct concat;
template<typename... T> using concat_t = typename concat<T...>::type;
template<> struct concat<string<>> {
    using type = string<>;
};
template<typename T> struct concat<T> {
    using type = T;
};
template<char... T, char... U> struct concat<string<T...>, string<U...>> {
    using type = string<T..., U...>;
};
template<typename T, typename U, typename... V> struct concat<T, U, V...> {
    using type = concat_t<concat_t<T, U>, concat_t<V...>>;
};

// repr = true to include quotes and such to make a roundtrip possible.
template<typename T, bool repr = false> struct print;
template<typename T, bool repr = false> using print_t = typename print<T, repr>::type;
template<typename T, bool repr = false> const char *printed = to_data<print_t<T, repr>>::array;

template<typename T, bool repr> struct print_cons;
template<typename T, bool repr> using print_cons_t = typename print_cons<T, repr>::type;

template<int val, bool negative> struct print_int;
template<int val, bool negative> using print_int_t = typename print_int<val, negative>::type;

template<> struct print<nil> { using type = decltype("nil"_str); };

template<typename CAR, typename CDR, bool repr> struct print<cons<CAR, CDR>, repr> {
    using type = concat_t<string<'('>, print_cons_t<cons<CAR, CDR>, repr>, string<')'>>; };


template<char... C> struct print<string<C...>> { using type = string<C...>; };
template<char... C> struct print<string<C...>, true> { using type = concat_t<string<'"'>, string<C...>, string<'"'>>; };

template<char... C, bool R> struct print<symbol<C...>, R> { using type = string<C...>; };
template<char C> struct print<value_type<char, C>> { using type = string<C>; };
template<char C> struct print<value_type<char, C>, true> { using type = concat_t<string<'\''>, string<C>, string<'\''>>; };

template<int val, bool R> struct print<value_type<int, val>, R> { using type = print_int_t<val, val < 0>; };

template<typename ARGS, typename BODY, typename ENV> struct print<lambda<ARGS, BODY, ENV>> {
    using type = decltype("<procedure>"_str);
};

//template<typename T, bool repr> struct print { using type = decltype("UNIMPL"_str); };

/*********************************************************** List Printer */

// Perhaps repr should always be true when printing something inside a list? (And when is repr false?)
template<typename CAR, typename CDR, bool repr> struct print_cons<cons<CAR, CDR>, repr> {
    using type = concat_t<print_t<CAR, repr>, string<' '>, print_cons_t<CDR, repr>>; };

template<typename CAR, bool repr> struct print_cons<cons<CAR, nil>, repr> {
    using type = print_t<CAR, repr>; };

// Dotted pair, a cdr is a non-cons
template<typename T, bool repr> struct print_cons {
    using type = concat_t<string<'.', ' '>, print_t<T, repr>>; };

/*********************************************************** Integer Printer */

template<int val> struct print_int_prefix { using type = print_int_t<val, false>; };
template<> struct print_int_prefix<0> { using type = string<>; };
template<int val> using print_int_prefix_t = typename print_int_prefix<val>::type;

template<int digit> using print_digit_t = string<'0' + digit>;

template<int val, bool negative> struct print_int {
    using type = concat_t<print_int_prefix_t<val / 10>, print_digit_t<val % 10>>;
};

template<int val> struct print_int<val, true> {
    using type = concat_t<string<'-'>, print_int_prefix_t<-val / 10>, print_digit_t<-val % 10>>;
};

}
