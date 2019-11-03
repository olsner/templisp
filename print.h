#pragma once

#include <cstring>
#include <iostream>

namespace {

/**************************************************************
Value Printers */
template <typename T>
struct print_val;

template <char val>
struct print_val<value_type<char, val> >
{
	char text;
	
	inline print_val()
	{
		text=val;
	}
};

/**************************************************************
Integer Printer */
template <int val>
struct print_int;

template <int val>
struct print_int_prefix
{
	print_int<val> prt;
};

template <>
struct print_int_prefix<0>
{};

template <int digit>
struct print_digit:
	public print_val<value_type<char, '0'+digit> >
{};

template <int val>
struct print_int:
	public print_int_prefix<val / 10>,
	public print_digit<val%10>
{
};

template <int val>
struct print_val<value_type<int, val> >
{
	print_int<val> text;
};

#define PRINT_STRING(_str) \
	print_string<sizeof(_str)-1, _str>
template <int strlen, const char *str>
struct print_string
{
	char text[strlen];
	
	inline print_string()
	{
		memcpy(text, str, strlen);
	}
};

/******************************************************
Cons Printer */
template <typename VAL>
struct print_cons;

extern const char print_list_sep[]=" ";
template <typename CAR, typename CDR>
struct print_cons<cons<CAR, CDR> >
{
	print_val<CAR> b;
	PRINT_STRING(print_list_sep) c;
	print_cons<CDR> d;
};

template <typename CAR>
struct print_cons<cons<CAR, nil> >
{
	print_val<CAR> b;
};

/*
	Dotted pair, a cdr is a non-cons
*/
extern const char print_cons_sep[]=". ";
template <typename VAL>
struct print_cons
{
	PRINT_STRING(print_cons_sep) a;
	print_val<VAL> b;
};

extern const char print_list_beg[]="(";
extern const char print_list_end[]=")";
template <typename CAR, typename CDR>
struct print_val<cons<CAR, CDR> >:
	public PRINT_STRING(print_list_beg),
	public print_cons<cons<CAR, CDR> >,
	public PRINT_STRING(print_list_end)
{};

template <typename T>
struct printable
{
	operator const char *() const
	{
		return (const char *)(T*)this;
	}
};

template <typename printer>
struct terminate:
	public printer,
	public print_val<value_type<char, 0> >,
	public printable<terminate<printer> >
{};

template <typename VAL>
struct print:
	public terminate<print_val<VAL> >
{
    print(VAL = VAL()) {}
};

template<typename T> std::ostream& operator<<(std::ostream& os, print<T> p) {
    return os << (const char*)p;
}

extern const char print_nil_text[]="nil";
struct print_nil
{
	PRINT_STRING(print_nil_text) text;
};

template <>
struct print_val<nil>:
	public print_nil
{};

template <>
struct print_val<string<> >
{};
template <char c, char... s>
struct print_val<string<c,s...> >:
	print_val<value_type<char,c> >,
	print_val<string<s...> >
{};

template <char c, char... cs>
struct print_val<symbol<c, cs...> >:
	print_val<value_type<char, c> >,
	print_val<symbol<cs...> >
{};
template <>
struct print_val<symbol<> >
{};

extern const char print_lambda[]="<procedure>";
template <typename ARGS, typename BODY, typename ENV>
struct print_val<lambda<ARGS, BODY, ENV> >:
	PRINT_STRING(print_lambda)
{};

template <int p>
struct print_val<ptr<p>>: print_val<value_type<char,'#'>>,print_int<p> {};

}
