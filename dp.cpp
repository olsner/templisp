#include "templ_lists2.h"

template <int len, class initVal=nil>
struct array;

template <class array, int n, class val>
struct array_set;

template <int w, int h, class initVal=nil>
struct matrix;

template <class matrix, int x, int y, class val>
struct matrix_set;

// countable traits
template<class val>
struct next;
template<class val>
struct prev;
template <class val>
struct is_first;

/******************************************************************************/
template <int len, class initVal>
struct array
{
	typedef cons<initVal, typename array<len-1, initVal>::value > value;
};

template <class initVal>
struct array<0, initVal>
{
	typedef nil value;
};

template <class array, class index, bool isLast>
struct array_at1;
template <class array, class index>
struct array_at;

template <class array, class index>
struct array_at1<array, index, false>
{
	typedef typename array_at<typename array::cdr, typename prev<index>::value>::value value;
};

template <class array, class index>
struct array_at1<array, index, true>
{
	typedef typename array::car value;
};

template <class array, class index>
struct array_at
{
	typedef typename array_at1<array, index, is_first<index>::value>::value value;
};

/******************************************************************************/
template <int w, int h, class initVal>
struct matrix
{
	typedef cons<typename array<w, initVal>::value, typename matrix<w, h-1, initVal>::value > value;
};

template <int w, class initVal>
struct matrix<w, 0, initVal>
{
	typedef nil value;
};

/******************************************************************************/
template <class array, int n, class val>
struct array_set
{
	typedef cons<
		typename array::car,
		typename array_set<typename array::cdr, n-1, val>::value> value;
};

template <class array, class val>
struct array_set<array, 0, val>
{
	typedef cons<val, typename array::cdr> value;
};

/******************************************************************************/
template <class m, int x, int y, class val>
struct matrix_set
{
	typedef cons<
		typename m::car,
		typename matrix_set<typename m::cdr, x, y-1, val>::value> value;
};

template <class matrix, int x, class val>
struct matrix_set<matrix, x, 0, val>
{
	typedef cons<
		typename array_set<typename matrix::car, x, val>::value,
		typename matrix::cdr> value;
};

/******************************************************************************/
// Integer/countable traits

template<int n>
struct next<INT(n)>
{
	typedef INT(n+1) value;
};

template<int n>
struct prev<INT(n)>
{
	typedef INT(n-1) value;
};

template<int n>
struct is_first<INT(n)>
{
	static const bool value = (n==0);
};

/******************************************************************************/
template <class functor, class arg, class start, class end>
struct map_range
{
	typedef typename functor::template apply<arg, start>::value nextArg;
	typedef typename map_range<functor, nextArg, typename next<start>::value, end>::value value;
};

template <class functor, class arg, class it>
struct map_range<functor, arg, it, it>
{
	typedef arg value;
};

template <class val>
struct array_set_functor
{
	template <class arg, class n>
	struct apply
	{
		typedef typename array_set<arg, n::value, val>::value value;
	};
};

template <int start=0>
struct all_integers;

template <int start>
struct all_integers
{
	typedef INT(start) car;
	typedef all_integers<start+1> cdr;
};

struct cons_functor
{
	template <class arg, class n>
	struct apply
	{
		typedef cons<next<n>, arg> value;
	};
};

#include <stdio.h>

int main()
{
	#define PRINT(...) { print<__VA_ARGS__> printed; printf(#__VA_ARGS__ ": %s.\n", (char *)printed); }
	
	typedef array<5, INT(2)>::value a1;
	PRINT(a1);
	typedef array_set<a1, 3, INT(3)>::value a2;
	PRINT(a2);
	typedef array_set_functor<INT(4)> set_4_func;
	PRINT(BOOL(is_first<prev<INT(1)>::value>::value));
	PRINT(array_at<all_integers<0>, INT(13)>::value);
}
