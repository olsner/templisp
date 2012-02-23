#include <string.h>

/*
templ_lists2.h: These lists have types as values all the way. This is to
allow templ_lisp to represent different kinds of values as different types
(also enabling using templates to match c++ type and c++ value of a lisp val)
*/

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
struct select_type<false, true_type, false_type>
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

struct nil
{
	typedef nil car;
	typedef nil cdr;
};

template <typename CAR, typename CDR>
struct cons
{
	typedef CAR car;
	typedef CDR cdr;
};

/*
	There are two kinds of values: ones that have an actual c++ constant value,
	and those that are represented by a type (i.e. conses). c++ constant values
	are wrapped in this class, other simply use their respective class as the
	"value type"
*/
template <typename T, T val>
struct value_type
{
	typedef T type;
	static const T value=val;
};

#define INT(_i) value_type<int, _i>
#define BOOL(_b) value_type<bool, _b>

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
	};
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
Bool Printer */
extern const char str_bool_false[]="false";
extern const char str_bool_true[]="true";

template <>
struct print_val<BOOL(false)>:
	PRINT_STRING(str_bool_false)
{};

template <>
struct print_val<BOOL(true)>:
	PRINT_STRING(str_bool_true)
{};

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
	inline operator char *()
	{
		return (char *)(T*)this;
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
{};

extern const char print_nil_text[]="nil";
struct print_nil
{
	PRINT_STRING(print_nil_text) text;
};

template <>
struct print_val<nil>:
	public print_nil
{};

template <typename CONS>
struct length
{
	static const uint value=1+length<typename CONS::cdr>::value;
};

template <>
struct length<nil>
{
	static const uint value=0;
};

/*
(defun my-reverse (lst &optional (out nil)) 
  (cond ((null lst) out)
        (t (my-reverse-aux (rest lst) (cons (first lst) out)))))*/
template <typename CONS, typename OUT=nil>
class reverse
{
	typedef typename CONS::car car;
	typedef typename CONS::cdr cdr;
public:
	typedef typename reverse<cdr, cons<car, OUT> >::value value;
};

template <typename OUT>
struct reverse<nil, OUT>
{
	typedef OUT value;
};

template <typename L1, typename L2>
class append
{
public:
	typedef cons<typename L1::car,
				typename append<typename L1::cdr, L2>::value>
		value;
};

template <typename L2>
struct append<nil, L2>
{
	typedef L2 value;
};
