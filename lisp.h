#include "utils.h"
#include "lists.h"
#include "print.h"
#include "heap.h"

template <const char *sym_val>
struct lisp_symbol
{};

#define SYM(_s) lisp_symbol<lisp_symbol_text_##_s>
#define DEFINE(_sym, _text) \
	extern const char lisp_symbol_text_##_sym[]=_text; \
	template <> \
	struct print_val<lisp_symbol<lisp_symbol_text_##_sym> > \
	{ \
		PRINT_STRING(lisp_symbol_text_##_sym) text; \
	}; \
	typedef SYM(_sym) _sym;

DEFINE(CONS, "cons")
DEFINE(QUOTE, "quote")
DEFINE(DEFINE, "define")
DEFINE(SET, "set")
DEFINE(LAMBDA, "lambda")
DEFINE(PROGN, "progn")
DEFINE(IF, "if")
//DEFINE(EQ, "eq")
DEFINE(T, "t");
DEFINE(null, "null");
DEFINE(CAR, "car");
DEFINE(CDR, "cdr");
DEFINE(PLUS, "+");
typedef nil NIL;

#define INT(_i) value_type<int, _i>

// User Symbols
DEFINE(A, "a")
DEFINE(B, "b")
DEFINE(C, "c")
DEFINE(D, "d")
DEFINE(F, "f")
DEFINE(G, "g")
DEFINE(X, "x")
DEFINE(APPEND, "append")
DEFINE(APPEND_2, "append-2")

template <typename HEAP, typename SP>
struct env_
{
	typedef HEAP heap;
	typedef SP sp; // pointer to current stack frame
};

template <typename H, typename SP, typename P>
struct peek<env_<H,SP>,P>
{
	typedef peek<H, P> peeked;
	typedef typename peeked::value value;
};
template <typename H, typename SP, typename P, typename V>
struct poke<env_<H,SP>,P,V>
{
	typedef poke<H, P, V> poked;
	typedef env_<typename poked::value,SP> value;
};
template <typename H, typename SP, typename V>
struct alloc<env_<H,SP>,V>
{
	typedef alloc<H,V> alloced;
	typedef env_<typename alloced::heap,SP> env;
	typedef typename alloced::value value;
};

template <typename ENV, typename SP>
struct re_sp;

template <typename H, typename O, typename SP>
struct re_sp<env_<H,O>,SP>
{
	typedef env_<H,SP> value;
};

template <typename F, typename ENV>
struct set_frame;
template <typename F, typename H, typename SP>
struct set_frame<F, env_<H,SP> >
{
	typedef env_<H,SP> env;
	typedef typename peek<env, SP>::value::cdr cdr;
	typedef typename poke<env, SP, cons<F,cdr> >::value value;
};
template <typename SYM, typename VAL, typename ENV>
class add_binding
{
	typedef typename ENV::sp sp;
	typedef typename peek<ENV, sp>::value::car first_frame;
	typedef cons<cons<SYM,VAL>,first_frame> new_first_frame;
public:
	typedef typename set_frame<new_first_frame, ENV>::value value;
};

template <typename FRAME, typename ENV>
class push_frame;
template <typename FRAME, typename H, typename SP>
class push_frame<FRAME, env_<H,SP> >
{
	typedef alloc<H, cons<FRAME,SP> > a;
public:
	typedef env_<typename a::heap, typename a::value> value;
};


template <typename ENV>
class pop_frame
{
public:
	typedef typename ENV::cdr value;
};

template <typename FORMALS, typename VALUES, typename ENV>
class bind_parameters
{
	typedef bind_parameters<typename FORMALS::cdr,
							typename VALUES::cdr,
							ENV> rest_bindings;
public:
	typedef add_binding<typename FORMALS::car,
						typename VALUES::car,
						rest_bindings> value;
};

template <typename ENV>
struct bind_parameters<nil, nil, ENV>
{
	typedef ENV value;
};

template <typename SYM>
struct no_binding_error;

template <typename ENV, typename SYM>
struct get_binding;

template <typename FRAME, typename SYM>
struct get_frame_binding;
template <typename REST_FRAME, typename VALUE, typename SYM>
struct get_frame_binding<cons<cons<SYM, VALUE>, REST_FRAME>, SYM>
{
	typedef VALUE value;
};
template <typename FIRST, typename REST_FRAME, typename SYM>
struct get_frame_binding<cons<FIRST, REST_FRAME>, SYM>
{
	typedef typename get_frame_binding<REST_FRAME, SYM>::value value;
};
template <typename SYM>
struct get_frame_binding<nil, SYM>
{
	typedef no_binding_error<SYM> value;
};

template <typename FRAME, typename SYM, typename VAL>
struct set_frame_binding;
template <typename REST, typename SYM, typename _, typename VAL>
struct set_frame_binding<cons<cons<SYM,_>,REST>,SYM,VAL>
{
	typedef cons<cons<SYM,VAL>,REST> value;
};
template <typename FIRST, typename REST, typename SYM, typename VAL>
struct set_frame_binding<cons<FIRST, REST>,SYM,VAL>
{
	typedef cons<FIRST,typename set_frame_binding<REST,SYM,VAL>::value> value;
};
template <typename SYM, typename VAL>
struct set_frame_binding<nil,SYM,VAL>
{
	typedef no_binding_error<SYM> value;
};

template <typename ENV, typename SP, typename SYM>
struct get_binding_int;

template <typename ENV, typename SYM>
struct get_binding_int<ENV, nil, SYM>
{
	typedef no_binding_error<SYM> value;
};

template <typename ENV, typename SP, typename SYM>
struct get_binding_int
{
	typedef typename peek<ENV, SP>::value sp;
	typedef typename get_frame_binding<typename sp::car, SYM>::value frameres;

	typedef typename select_type<
		same_type<frameres, no_binding_error<SYM> >::value,
		typename get_binding_int<ENV, typename sp::cdr, SYM>::value,
		frameres>::type value;
};

template <typename ENV, typename SYM>
struct get_binding
{
	typedef typename get_binding_int<ENV, typename ENV::sp, SYM>::value value;
};

template <typename ENV, typename SP, typename SYM, typename VAL>
struct set_binding_int
{
	typedef typename peek<ENV, SP>::value sp;
	typedef typename get_frame_binding<typename sp::car, SYM>::value get_res;

	typedef typename set_frame_binding<typename sp::car, SYM, VAL>::value frameres;
	typedef cons<frameres, typename sp::cdr> new_frame;

	typedef typename select_type<
		same_type<get_res, no_binding_error<SYM> >::value,
		typename set_binding_int<ENV, typename sp::cdr, SYM, VAL>::value,
		typename poke<ENV, SP, new_frame>::value>::type value;
};
template <typename ENV, typename SYM, typename VAL>
struct set_binding_int<ENV,nil,SYM,VAL>
{
	typedef typename add_binding<SYM,VAL,ENV>::value value;
};

template <typename SYM, typename VAL, typename ENV>
struct set_binding
{
	typedef typename set_binding_int<ENV, typename ENV::sp, SYM, VAL>::value value;
};


template <typename ARGS, typename BODY, typename SP>
struct lambda
{
	typedef ARGS args;
	typedef BODY body;
	typedef SP sp;
};
template <typename ARGS, typename BODY, typename ENV>
struct print_val<lambda<ARGS, BODY, ENV> >:
	print_val<typename append<list<LAMBDA, ARGS>,BODY>::value>
{};

/*
	(eval form env)
	form is a lisp++ form to evaluate
	env is an association list of variables with their respective values
*/
template <typename FORM, typename ENV>
struct eval;

/* Speed-up macros for common operations */

template <typename LIST>
struct first
{
	typedef typename LIST::car value;
};
#define FIRST(...) typename first<__VA_ARGS__>::value
template <typename LIST>
struct second
{
	typedef FIRST(typename LIST::cdr) value;
};
#define SECOND(...) typename second<__VA_ARGS__>::value

// value
template <typename T, T val, typename ENV>
struct eval<value_type<T, val>, ENV>
{
	typedef value_type<T, val> value;
	typedef ENV env;
};

// variable (symbol)
template <const char *SYM, typename ENV>
struct eval<lisp_symbol<SYM>, ENV>
{
	typedef typename get_binding<ENV, lisp_symbol<SYM> >::value value;
	typedef ENV env;
};

// nil
template <typename ENV>
struct eval<nil, ENV>
{
	typedef nil value;
	typedef ENV env;
};

// (cons REST)
template <typename REST, typename ENV>
class eval<cons<CONS, REST>, ENV>
{
	typedef eval<FIRST(REST), ENV> fir;
	typedef eval<SECOND(REST), typename fir::env> sec;
	typedef alloc<typename sec::env, cons<typename fir::value, typename sec::value> > alloced;
public:
	typedef typename alloced::value value;
	typedef typename alloced::env env;
};

template <typename ENV, typename V>
struct car;

template <typename ENV, typename CAR, typename CDR>
struct car<ENV, cons<CAR,CDR> >
{
	typedef CAR value;
};

template <typename ENV, typename P>
struct car
{
	typedef typename car<ENV, typename peek<ENV, P>::value>::value value;
};

// (car ARG)
template <typename ARG, typename ENV>
struct eval<cons<CAR, cons<ARG, nil> >, ENV>
{
	typedef eval<ARG, ENV> eval1;
	typedef typename eval1::env env;
	typedef typename car<env, typename eval1::value>::value value;
};

// (cdr ARG)
template <typename ARG, typename ENV>
struct eval<cons<CDR, cons<ARG, nil> >, ENV>
{
	typedef typename eval<ARG, ENV>::value::cdr value;
	typedef ENV env;
};

// (null ARG)
template <typename ARG, typename ENV>
class eval<cons<null, cons<ARG, nil> >, ENV>
{
	typedef typename eval<ARG, ENV>::value arg;
public:
	static const bool val = same_type<arg, nil>::value;
	typedef typename select_type<val, T, nil>::type value;
	typedef ENV env;
};

// (quote REST)
template <typename REST, typename ENV>
struct eval<cons<QUOTE, cons<REST, nil> >, ENV>
{
	typedef REST value;
	typedef ENV env;
};

// (if TEST TRUE_CLAUSE FALSE_CLAUSE)
template <typename TEST, typename T, typename F, typename ENV>
struct eval<cons<IF, cons<TEST, cons<T, cons<F, nil> > > >, ENV>
{
	typedef typename select_type<
		same_type<typename eval<TEST, ENV>::value, nil>::value,
		eval<F, ENV>, eval<T, ENV> >::type result_eval;
	typedef typename result_eval::value value;
	typedef typename result_eval::env env;
};

// (set (cdr EXPR) FORM)
template <typename EXPR, typename FORM, typename ENV>
struct eval<cons<SET,cons<cons<CDR,cons<EXPR,nil> >,cons<FORM,nil> > >,ENV>
{
	typedef eval<EXPR, ENV> result1;
	typedef typename result1::value p;
	typedef eval<FORM, typename result1::env> result2;
	typedef typename result2::value value;
	typedef typename peek<typename result2::env, p>::value oldcons;
	typedef cons<typename oldcons::car, value> newcons;
	typedef typename poke<typename result2::env, p, newcons>::value env;
};

// (set VAR FORM)
template <typename VAR, typename FORM, typename ENV>
struct eval<cons<SET, cons<VAR, cons<FORM, nil> > >, ENV>
{
	typedef eval<FORM, ENV> result;
	typedef typename result::value value;
	typedef typename set_binding<VAR, value, typename result::env>::value env;
};

// (define (fun args...) BODY)
template <typename FUN, typename ARGS, typename BODY, typename ENV>
struct eval<cons<DEFINE, cons<cons<FUN, ARGS>, BODY> >, ENV>
{
	typedef eval<cons<LAMBDA, cons<ARGS, BODY> >, ENV> result;
	typedef typename result::value value;
	typedef typename set_binding<FUN, value, typename result::env>::value env;
};

// (define VAR FORM)
template <const char* NAME, typename FORM, typename ENV>
struct eval<cons<DEFINE, cons<lisp_symbol<NAME>, cons<FORM, nil> > >, ENV>
{
	typedef lisp_symbol<NAME> VAR;
	typedef eval<FORM, ENV> result;
	typedef typename result::value value;
	typedef typename set_binding<VAR, value, typename result::env>::value env;
};

// (progn FIRST REST) => (eval first), (progn REST)
template <typename FIRST, typename REST, typename ENV>
class eval<cons<PROGN, cons <FIRST, REST> >, ENV>
{
	typedef eval<FIRST, ENV> first_result;
	typedef typename first_result::env first_env;
	typedef eval<cons<PROGN, REST>, first_env> result;
//	typedef first_result result;
public:
	typedef typename result::value value;
	typedef typename result::env env;
};

// (progn LAST)
template <typename LAST, typename ENV>
class eval<cons<PROGN, cons<LAST, nil> >, ENV>
{
	typedef eval<LAST, ENV> result;
public:
	typedef typename result::value value;
	typedef typename result::env env;
};

template <typename FORMALS, typename ACTUALS, typename ENV>
class create_frame_eval
{
	typedef eval<typename ACTUALS::car, ENV> result;
	typedef cons<typename FORMALS::car, typename result::value> first_binding;
	typedef create_frame_eval<typename FORMALS::cdr,
							  typename ACTUALS::cdr,
							  typename result::env> rest_bindings;
public:
	typedef cons<first_binding, typename rest_bindings::value> value;
};

template <typename ENV>
class create_frame_eval<nil, nil, ENV>
{
public:
	typedef nil value;
};

template <typename FUN, typename ACTUALS, typename ENV>
class apply;

template <typename FORMALS, typename BODY, typename LEXSP,
		typename ACTUALS, typename H, typename SP>
class apply<lambda<FORMALS, BODY, LEXSP>, ACTUALS, env_<H,SP> >
{
	typedef env_<H,SP> ENV;
	typedef env_<H,LEXSP> LEXENV;
	typedef typename create_frame_eval<FORMALS, ACTUALS, ENV>::value new_frame;
	typedef typename push_frame<new_frame, LEXENV>::value subenv;
	typedef eval<cons<PROGN, BODY>, subenv> result;
public:
	typedef typename result::value value;
	typedef typename re_sp<typename result::env,SP>::value env;
};

template <typename ENV>
class apply<PLUS, nil, ENV>
{
public:
	typedef INT(0) value;
	typedef ENV env;
};

template <typename ACTUALS, typename ENV>
class apply<PLUS, ACTUALS, ENV>
{
	template<typename A, typename B>
	struct plus;

	template<int a, int b>
	struct plus<INT(a), INT(b)>
	{
		typedef INT(a+b) value;
	};
	typedef eval<typename ACTUALS::car, ENV> result_head;
	typedef apply<PLUS, typename ACTUALS::cdr, typename result_head::env> result_tail;
public:
	typedef typename plus<typename result_head::value, typename result_tail::value>::value value;
	typedef ENV env;
};

// (lambda ...)
template <typename ARGS, typename BODY, typename ENV>
class eval<cons<LAMBDA, cons<ARGS, BODY> >, ENV>
{
public:
	typedef lambda<ARGS, BODY, typename ENV::sp> value;
	typedef ENV env;
};

// (FUN ACTUALS)
template <typename FUN, typename ACTUALS, typename ENV>
class eval<cons<FUN, ACTUALS>, ENV>
{
	typedef eval<FUN, ENV> fun;
	typedef apply<typename fun::value, ACTUALS, typename fun::env> result;
public:
	typedef typename result::value value;
	typedef typename result::env env;
};

typedef env_<heap<>,nil> really_empty_env;
typedef push_frame<nil, really_empty_env>::value empty_env;
typedef add_binding<PLUS, PLUS, empty_env>::value initial_env;

