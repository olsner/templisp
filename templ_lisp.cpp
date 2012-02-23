#include "utils.h"
#include "lists.h"
#include "print.h"

#include <stdio.h>
#undef NULL

typedef cons<nil, nil> initial_env;

template <typename BINDING, typename ENV>
class add_binding
{
	typedef typename ENV::car first_frame;
	typedef typename ENV::cdr rest_frames;
	typedef cons<BINDING, first_frame> new_first_frame;
public:
	typedef cons<new_first_frame, rest_frames> value;
};

template <typename FRAME, typename ENV>
class push_frame
{
public:
	typedef cons<FRAME, ENV> value;
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
	typedef cons<typename FORMALS::car, typename VALUES::car> first_binding;
	typedef bind_parameters<typename FORMALS::cdr,
							typename VALUES::cdr,
							ENV> rest_bindings;
public:
	typedef add_binding<first_binding, rest_bindings> value;
};

template <typename ENV>
struct bind_parameters<nil, nil, ENV>
{
	typedef ENV value;
};

struct no_binding_error
{};

template <typename ENV, typename SYM>
struct get_binding;

template <typename FRAME, typename SYM>
struct get_frame_binding;

template <typename FIRST, typename REST_FRAME, typename SYM>
struct get_frame_binding<cons<FIRST, REST_FRAME>, SYM>
{
	typedef typename get_frame_binding<REST_FRAME, SYM>::value value;
};

template <typename REST_FRAME, typename VALUE, typename SYM>
struct get_frame_binding<cons<cons<SYM, VALUE>, REST_FRAME>, SYM>
{
	typedef VALUE value;
};

template <typename SYM>
struct get_frame_binding<nil, SYM>
{
	typedef no_binding_error value;
};

template <typename ENV, typename SYM>
struct get_binding_int;

template <typename SYM>
struct get_binding_int<nil, SYM>
{
	typedef no_binding_error value;
};

template <typename ENV, typename SYM>
struct get_binding_int
{
	typedef typename get_frame_binding<typename ENV::car, SYM>::value frameres;
	
	typedef typename select_type<
		same_type<frameres, no_binding_error>::value,
		typename get_binding_int<typename ENV::cdr, SYM>::value,
		frameres>::type value;
};

template <typename ENV, typename SYM>
class get_binding
{
	typedef typename get_binding_int<ENV, SYM>::value result;
	
	//BOOST_STATIC_ASSERT(!(same_type<result, no_binding_error>::value));
public:
	typedef result value;
};

/*
	(eval form env)
	form is a lisp++ form to evaluate
	env is an association list of variables with their respective values
*/
template <typename FORM, typename ENV>
struct eval;

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
	};

/*
	(CONS a b) => cons<A, B>
*/
DEFINE(CONS, "cons")
/*
	(QUOTE a) => a
	(QUOTE (a b c)) => (a b c)
*/
DEFINE(QUOTE, "quote")
/*
	(set a (form)) => evaluate form, assign value to variable a
*/
DEFINE(SET, "set")
/*
	(lambda (argnames...) forms...)
*/
DEFINE(LAMBDA, "lambda")
DEFINE(PROGN, "progn")
DEFINE(IF, "if")
//DEFINE(EQ, "eq")
DEFINE(T, "t");
DEFINE(NULL, "null");
DEFINE(CAR, "car");
DEFINE(CDR, "cdr");

#define INT(_i) value_type<int, _i>

// User Symbols
DEFINE(A, "a")
DEFINE(B, "b")
DEFINE(C, "c")
DEFINE(APPEND, "append")
DEFINE(APPEND_2, "append-2")
/*enum user_symbol
{
	a, b, c, d
};
#define USYM(_s) value_type<user_symbol, _s>*/

template <typename A, typename B>
struct list2
{
	typedef cons<A, cons<B, nil> > value;
};

template <typename A, typename B, typename C>
struct list3
{
	typedef cons<A, typename list2<B, C>::value> value;
};

template <typename A, typename B, typename C, typename D>
struct list4
{
	typedef cons<A, typename list3<B, C, D>::value> value;
};

/* Speed-up macros for common operations */

#define LIST1(...) cons<__VA_ARGS__, nil>
#define LIST2(...) list2<__VA_ARGS__>::value
#define LIST3(...) list3<__VA_ARGS__>::value
#define LIST4(...) list4<__VA_ARGS__>::value

#define CALL1(_sym, ...) LIST2(SYM(_sym), __VA_ARGS__)
#define CALL2(_sym, _arg1, _arg2) LIST3(SYM(_sym), _arg1, _arg2)
#define CALL3(_sym, _arg1, _arg2, _arg3) LIST4(SYM(_sym), _arg1, _arg2, _arg3)

#define Q(_form) CALL1(QUOTE, _form)

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
class eval<cons<SYM(CONS), REST>, ENV>
{
	typedef eval<FIRST(REST), ENV> fir;
	typedef eval<SECOND(REST), typename fir::env> sec;
public:
	typedef cons<typename fir::value, typename sec::value> value;
	typedef typename sec::env env;
};

// (car ARG)
template <typename ARG, typename ENV>
struct eval<cons<SYM(CAR), cons<ARG, nil> >, ENV>
{
	typedef typename eval<ARG, ENV>::value::car value;
	typedef ENV env;
};

// (cdr ARG)
template <typename ARG, typename ENV>
struct eval<cons<SYM(CDR), cons<ARG, nil> >, ENV>
{
	typedef typename eval<ARG, ENV>::value::cdr value;
	typedef ENV env;
};

// (null ARG)
template <typename ARG, typename ENV>
class eval<cons<SYM(NULL), ARG>, ENV>
{
	typedef typename eval<typename ARG::car, ENV>::value arg;
public:
	typedef select_type<same_type<arg, nil>::value,
						SYM(T),
						nil> value;
	typedef ENV env;
};

// (quote REST)
template <typename REST, typename ENV>
struct eval<cons<SYM(QUOTE), cons<REST, nil> >, ENV>
{
	typedef REST value;
	typedef ENV env;
};

// (if TEST TRUE_CLAUSE FALSE_CLAUSE)
template <typename TEST, typename T, typename F, typename ENV>
struct eval<cons<SYM(IF), cons<TEST, cons<T, cons<F, nil> > > >, ENV>
{
	typedef typename select_type<
		same_type<typename eval<TEST, ENV>::value, nil>::value,
		eval<F, ENV>, eval<T, ENV> >::type result_eval;
	typedef typename result_eval::value value;
	typedef typename result_eval::env env;
};

// (set VAR FORM)
template <typename VAR, typename FORM, typename ENV>
struct eval<cons<SYM(SET), cons<VAR, cons<FORM, nil> > >, ENV>
{
	typedef eval<FORM, ENV> result;
	typedef typename result::value value;
	typedef typename add_binding<cons<VAR, value>,
								 typename result::env>::value env;
};

// (progn FIRST REST) => (eval first), (progn REST)
template <typename FIRST, typename REST, typename ENV>
class eval<cons<SYM(PROGN), cons <FIRST, REST> >, ENV>
{
	typedef eval<FIRST, ENV> first_result;
	typedef typename first_result::env first_env;
	typedef eval<cons<SYM(PROGN), REST>, first_env> result;
//	typedef first_result result;
public:
	typedef typename result::value value;
	typedef typename result::env env;
};

// (progn LAST)
template <typename LAST, typename ENV>
class eval<cons<SYM(PROGN), cons<LAST, nil> >, ENV>
{
	typedef eval<LAST, ENV> result;
public:
	typedef typename result::value value;
	typedef typename result::env env;
};

template <typename FORMALS, typename ACTUALS, typename ENV>
class create_frame_eval
{
//	BOOST_STATIC_ASSERT(
//		!(same_type<ACTUALS, nil>::value) && !(same_type<FORMALS, nil>::value));

	typedef eval<typename ACTUALS::car, ENV> result;
	typedef cons<typename FORMALS::car, typename result::value> first_binding;
	typedef create_frame_eval<typename FORMALS::cdr,
							  typename ACTUALS::cdr,
							  typename result::env> rest_bindings;
public:
	typedef cons<first_binding, typename rest_bindings::value> value;
};

/*template <typename FORMALS, typename ENV>
class create_frame_eval<FORMALS, nil, ENV>
{
	BOOST_STATIC_ASSERT(false);
};

template <typename ACTUALS, typename ENV>
class create_frame_eval<nil, ACTUALS, ENV>
{
	BOOST_STATIC_ASSERT(false);
};*/

template <typename ENV>
class create_frame_eval<nil, nil, ENV>
{
public:
	typedef nil value;
};

template <typename LAMBDA, typename ACTUALS, typename ENV>
class apply
{
	typedef SECOND(LAMBDA) formals;
	typedef typename LAMBDA::cdr::cdr body;
	typedef typename create_frame_eval<formals, ACTUALS, ENV>::value new_frame;
	typedef typename push_frame<new_frame, ENV>::value subenv;
	
	typedef eval<cons<SYM(PROGN), body>, subenv> result;
public:
	typedef typename result::value value;
	typedef typename pop_frame<typename result::env>::value env;
};

// ((lambda LAMBDA_REST) ACTUALS)
/*template <typename LAMBDA_REST, typename ACTUALS, typename ENV>
class eval<cons<cons<SYM(LAMBDA), LAMBDA_REST>, ACTUALS>, ENV>
{
	typedef apply<cons<SYM(LAMBDA), LAMBDA_REST>, ACTUALS, ENV> result;
public:
	typedef typename result::value value;
	typedef typename result::env env;
};*/

// (lambda ...)
template <typename LAMBDA_REST, typename ENV>
class eval<cons<SYM(LAMBDA), LAMBDA_REST>, ENV>
{
public:
	typedef cons<SYM(LAMBDA), LAMBDA_REST> value;
	typedef ENV env;
};

// (SYM ACTUALS)
template <typename SYM, typename ACTUALS, typename ENV>
class eval<cons<SYM, ACTUALS>, ENV>
{
	//typedef typename get_binding<ENV, SYM>::value function_form;
	typedef typename eval<SYM, ENV>::value function_form;
	typedef apply<function_form, ACTUALS, ENV> result;
public:
	typedef typename result::value value;
	typedef typename result::env env;
};

int main()
{
	// (cons 3 (quote 5 cons 2))
/*	typedef LIST3(SYM(CONS),
				INT(3),
				LIST4(SYM(QUOTE), INT(5), SYM(CONS), INT(2))) prog;*/
	// (if nil (cons 5 2) (quote 2 5))
/*	typedef LIST4(SYM(IF), SYM(T),
				LIST3(SYM(CONS), INT(5), INT(2)),
				LIST3(SYM(QUOTE), INT(2), INT(5))) prog;*/
	// (if t (quote 5 2) (quote 2 5))
	/*typedef LIST4(SYM(IF), SYM(T),
				LIST3(SYM(QUOTE), INT(5), INT(2)),
				LIST3(SYM(QUOTE), INT(2), INT(5))) prog;*/
	// (set a (set b (quote 5 2)))
/*	typedef LIST3(SYM(SET), SYM(A),
							LIST3(SYM(SET),
								  SYM(B),
								  LIST3(SYM(QUOTE), INT(5), INT(2)))) prog;*/
	// (progn (set a 5) a)
//	typedef LIST3(SYM(PROGN), LIST3(SYM(SET), SYM(A), INT(5)), SYM(A)) prog;
	// ((lambda (a b) (cons a b)) 5 2)
	typedef LIST3(LIST3(SYM(LAMBDA),
						LIST2(SYM(A), SYM(B)),
						CALL2(CONS, SYM(A), SYM(B))),
				  INT(5), INT(2)) inline_lambda;
	/* (set append
			(lambda (a b)
			  (if (null a)
			  		b
				(cons (car a) (append (cdr a) b))))) */
	typedef LIST3(SYM(SET), SYM(APPEND),
							LIST3(SYM(LAMBDA), LIST2(SYM(A), SYM(B)),
								LIST4(SYM(IF),
								      LIST2(SYM(NULL), SYM(A)),
									  SYM(B),
								      LIST3(SYM(CONS),
								            LIST2(SYM(CAR), SYM(A)),
											LIST3(SYM(APPEND),
											      LIST2(SYM(CDR), SYM(A)),
											      SYM(B)))))) set_append;
	// (append (quote (nil)) (quote (4 5 6)))
	typedef LIST3(SYM(APPEND),
				  LIST2(SYM(QUOTE), LIST2(INT(1), INT(2))),
				  LIST2(SYM(QUOTE), LIST3(INT(4), INT(5), INT(6)))) use_append;
	// (set a 5)
	typedef CALL2(SET, SYM(A), INT(5)) set_a;
	// (set c (lambda (a b) (cons <inline_lamba> (car b))))
	typedef LIST3(SYM(SET),
				  SYM(C),
				  LIST3(SYM(LAMBDA), LIST2(SYM(A), SYM(B)),
				  	LIST3(SYM(CONS), inline_lambda, LIST2(SYM(CAR), SYM(B))))) set_c_lambda;
	// (c 1 (quote (2 3)))
	typedef LIST3(SYM(C), INT(1), LIST2(SYM(QUOTE), LIST2(INT(7), INT(8)))) run_c;
	//typedef LIST4(SYM(PROGN), set_c_lambda, set_a, run_c) prog;
	//typedef LIST3(SYM(PROGN), set_append, use_append) prog;
	//typedef CALL2(PROGN, CALL2(SET, SYM(A), nil), CALL1(NULL, SYM(A))) prog;
	
	typedef set_append prog;
	//typedef use_append prog2;
	typedef LIST3(SYM(APPEND),
				  LIST2(SYM(QUOTE), LIST1(INT(1))),
				  LIST2(SYM(QUOTE), LIST3(INT(4), INT(5), INT(6)))) prog2;
	typedef eval<prog, initial_env> eval_result1;
	typedef eval<prog2, eval_result1::env> eval_result;
	typedef eval_result::value value;
	
	#define PRINT(_type) { print<_type> printed; printf(#_type ": %s.\n", (char *)printed); }
	
	PRINT(prog);
	//PRINT(prog2);
	PRINT(eval_result1::value);
	PRINT(eval_result1::env);
	PRINT(prog2);
	PRINT(eval_result::value);
	PRINT(eval_result::env);
}
