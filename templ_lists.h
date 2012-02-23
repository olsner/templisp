#include <string.h>

typedef unsigned int uint;

template <typename T, bool b, T true_val, T false_val>
struct select_value;

template <typename T, T true_val, T false_val>
struct select_value<T, true, true_val, false_val>
{
	static const T value=true_val;
};

template <typename T, T true_val, T false_val>
struct select_value<T, false, true_val, false_val>
{
	static const T value=false_val;
};

template <bool b, typename true_type, typename false_type>
struct select_type;

template <typename true_type, typename false_type>
struct select_type<true, true_type, false_type>
{
	typedef true_type type;
};

template <typename true_type, typename false_type>
struct select_type<false, true_type, false_type>: public false_type
{
	typedef false_type type;
};

template <typename A, typename B>
struct same_type
{
	static const bool value=false;
};

template <typename A>
struct same_type<A, A>
{
	static const bool value=true;
};

template <typename T>
struct wrapper
{
	typedef T type;
};

struct nil_base
{};

template <typename T>
struct nil: public nil_base
{
	typedef T car_type;
	static const T car=T();
	typedef nil<T> cdr;
};

typedef nil<int> inil;

template <typename T, T CAR, typename CDR>
struct cons
{
	typedef T car_type;
	static const T car=CAR;
	typedef CDR cdr;
};

template <int CAR, typename CDR>
struct icons: public cons<int, CAR, CDR>
{};

/**************************************************************
Value Printers */
template <typename T, T val>
struct print_val;

template <char val>
struct print_val<char, val>
{
	char text;
	
	inline print_val()
	{
		text=val;
	};
};

/**************************************************************
Integer Printer */
template <int val>
struct print_int;

template <>
struct print_int<0>
{};

template <int digit>
struct print_digit:
	public print_val<char, '0'+digit>
{};

template <int val>
struct print_int:
	public print_int<val / 10>,
	public print_digit<val%10>
{
};

template <int val>
struct print_val<int, val>
{
	print_int<val> text;
};

#define PRINT_STRING(_str, _dummy) \
	print_string<sizeof(_str)-1, _str, _dummy>
template <int strlen, const char *str, typename DUMMY>
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
extern const char print_cons_beg[]="(";
extern const char print_cons_sep[]=" . ";
extern const char print_cons_end[]=")";
template <typename CONS>
struct print_cons
{
	PRINT_STRING(print_cons_beg, CONS) a;
	print_val<typename CONS::car_type, CONS::car> b;
	PRINT_STRING(print_cons_sep, CONS) c;
	print_cons<typename CONS::cdr> d;
	PRINT_STRING(print_cons_end, CONS) e;
};

template <typename T>
struct printable
{
	inline operator char *()
	{
		return (char *)(T*)this;
	}
};

template <typename printer>
struct terminate:
	public printer,
	public print_val<char, 0>,
	public printable<terminate<printer> >
{};

template <typename CONS>
struct print:
	public terminate<print_cons<CONS> >
{};

extern const char print_nil_text[]="nil";
struct print_nil
{
	PRINT_STRING(print_nil_text, print_nil) text;
};

template <typename T>
struct print_cons<nil<T> >:
	public print_nil
{};

/*******************************************************************************
Sorted List Algorithms */
template <typename CONS, typename CONS::car_type val>
struct insert;

template <typename CONS, typename CONS::car_type val>
struct insert:
	public select_type<(val < CONS::car),
		/*true */cons<typename CONS::car_type, val, CONS>,
		/*false*/cons<typename CONS::car_type, CONS::car,
			insert<typename CONS::cdr, val> > >
{};

template <typename T, T val>
struct insert<nil<T>, val>:
	public cons<T, val, nil<T> >
{};

template <typename CONS, typename CONS::car_type val>
struct member
{
	static const bool value=(val == CONS::car)?
		true
		:member<typename CONS::cdr, val>::value;
};

template <typename car_type, car_type val>
struct member<nil<car_type>, val>
{
	static const bool value=false;
};

template <typename CONS, typename CONS::car_type val, bool b=(val == CONS::car)>
struct remove;

template <typename car_type, car_type val, bool b>
struct remove<nil<car_type>, val, b>
{
	typedef nil<car_type> type;
};

template <typename CONS, typename CONS::car_type val>
struct remove<CONS, val, true>
{
	typedef typename CONS::cdr type;
};

template <typename CONS, typename CONS::car_type val>
struct remove<CONS, val, false>
{
	typedef cons<typename CONS::car_type, CONS::car,
		typename remove<typename CONS::cdr, val>::type > type;
};

template <typename CONS>
struct length
{
	static const uint value=1+length<typename CONS::cdr>::value;
};

template <typename car_type>
struct length<nil<car_type> >
{
	static const uint value=0;
};

/*
(defun my-reverse-aux (lst out) 
  (cond ((null lst) out)
        (t (my-reverse-aux (rest lst) (cons (first lst) out)))))*/
template <typename CONS, typename OUT>
class reverse_aux
{
	typedef typename CONS::car_type car_type;
	typedef typename CONS::cdr cdr;
public:
	typedef typename reverse_aux<cdr, cons<car_type, CONS::car, OUT> >::type type;
};

template <typename car_type, typename OUT>
struct reverse_aux<nil<car_type>, OUT>
{
	typedef OUT type;
};

template <typename CONS>
struct reverse
{
	typedef typename reverse_aux<CONS, nil<typename CONS::car_type> >::type type;
};

template <typename L1, typename L2>
class append
{
	typedef typename L1::car_type car_type;
public:
	typedef cons<car_type,
		L1::car,
		typename append<typename L1::cdr, L2>::type> type;
};

template <typename car_type, typename L2>
struct append<nil<car_type>, L2>
{
	typedef L2 type;
};
