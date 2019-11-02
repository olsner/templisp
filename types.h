#pragma once

namespace {

// TODO Move reification into a separate template so each struct here doesn't need an extra field.
typedef struct ob_ *ob;

struct nil
{
	typedef nil car;
	typedef nil cdr;
	static ob reified;
};

template <typename CAR, typename CDR>
struct cons
{
	typedef CAR car;
	typedef CDR cdr;
	static ob reified;
};

template <typename T, T val>
struct value_type
{
	static const T value=val;
	static ob reified;
};

#define INT(_i) value_type<int, _i>

template <char... sym>
struct symbol
{
	static ob reified;
};

}
