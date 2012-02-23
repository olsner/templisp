
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

