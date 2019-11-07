#include <iostream>
#include <string_view>

using namespace std;

struct nil {};
template<typename CAR, typename CDR> struct cons {
    cons(CAR=CAR(), CDR=CDR());
};
template<typename T> struct quote {
    quote(T = T());
};
template<char...> struct symbol {};

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

#if 0
// TODO Match and strip more whitespace
template<char... STR> struct parse<' ', STR...>: parse<STR...> {};
template<char... STR> struct parse<'(', STR...>: parse_open_list<STR...> {};
template<char... STR> struct parse<'\'', STR...> {
    using p = parse<STR...>;
    using type = quote<typename p::type>;
    using tail = typename p::tail;
};
// template<char... STR> struct parse<'\"', STR...> // TODO Strings
// template<char C, char... STR, typename = enable_if_t<is_digit(C)>> struct parse<C, STR...>
// template<char C, char... STR, typename = enable_if_t<is_initial_symchar(C)>> struct parse<C, STR...>

template <char... STR> struct parse_open_list<' ', STR...>: parse_open_list<STR...> {};
template <char... STR> struct parse_open_list<')', STR...> {
    using type = nil;
    using tail = string_holder<STR...>;
};

template<typename T, char... TAIL> struct partial {};

constexpr size_t skip_ws(string_view s, size_t pos) {
    return
        pos >= s.size() ? pos :
        s[pos] == ' ' ? skip_ws(s, pos + 1) :
        pos;
}

template<typename Cont> constexpr auto parse_open_list(string_view s, Cont&& k, size_t pos) {
    constexpr size_t skip = skip_ws(s, pos);
    if (s[skip] == ')') {
        return k(nil(), s, skip + 1);
    } else {
        return parse_lisp(s, [&k](auto car, string_view s, size_t pos) {
            return parse_open_list(s, [car, &k](auto cdr, string_view s, size_t pos) {
                k(cons(car, cdr), s, pos);
            }, pos);
        }, pos);
    }
}

template<typename Cont> constexpr auto parse_lisp(string_view s, Cont&& k, size_t pos = 0) {
    if (pos >= s.size()) {
        return k(nil(), s, pos);
    }
    else {
        constexpr size_t skip = skip_ws(s, pos);
        if (s[skip] == ' ') { // TODO is whitespace, but must be constexpr
            return parse_lisp(s, k, skip + 1);
        } else if (s[skip] == '(') {
            return parse_open_list(s, k, skip + 1);
        } else if (s[skip] == '\'') {
            return parse_lisp(s, skip + 1, [&k](auto val, string_view s, size_t pos) { k(quote(val), s, pos); });
        }
    }
}

constexpr auto parse_lisp(string_view s) {
    return parse_lisp(s, [](auto val, ...) { return val; });
}

int main()
{
#define TEST(lit) \
    { \
        constexpr auto expr = parse_lisp(lit); \
        std::cout << expr << std::endl; \
    }

    TEST("()");
    TEST(" ()");
    TEST(" ( )");
    TEST("'()");
    TEST("(() ())");
    TEST("(() () ())");
}
#endif

constexpr bool is_digit(char c) {
    return '0' <= c && c <= '9';
}

template<char... STR> struct string_holder;
template <typename S, int acc = 0, typename Enable = void> struct parse_num;
template <typename S, typename Enable = void> struct parse;

template<char C, char... STR>
struct parse<string_holder<C, STR...>, enable_if_t<is_digit(C)>>: parse_num<string_holder<C, STR...>> {};

template<int acc> struct parse_num<string_holder<>, acc> { static constexpr int value = acc; };
template<int acc, char C, char... STR>
struct parse_num<string_holder<C, STR...>, acc, enable_if_t<is_digit(C)>>:
    parse_num<string_holder<STR...>, acc * 10 + (C - '0')> {};

template<typename C, C... STR> constexpr auto operator"" _num() { return parse<string_holder<STR...>>::value; }

int main() {
    constexpr auto x = "1234"_num;
    constexpr auto y = "056789"_num;
    std::cout << x << " " << y << std::endl;
}
