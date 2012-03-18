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
	print_val<typename list<LAMBDA, ARGS>::value>
{};

template <typename EXPR>
struct analyze;
template <typename FUN, typename ACTUALS>
class apply;

template <typename T>
struct analyze_many;

template <typename CAR, typename CDR>
struct analyze_many<cons<CAR,CDR> >
{
	typedef cons<analyze<CAR>, typename analyze_many<CDR>::value> value;
};
template <> struct analyze_many<nil> { typedef nil value; };

// Helpers for evaluations without side-effects
template <typename ENV>
struct pureval
{
	typedef ENV env;
};

// value
template <typename T, T val>
struct analyze<value_type<T, val>>
{
	template <typename ENV> struct eval: pureval<ENV>
	{
		typedef value_type<T, val> value;
	};
};

// variable (symbol)
template <const char *SYM>
struct analyze<lisp_symbol<SYM> >
{
	template <typename ENV> struct eval: pureval<ENV>
	{
		typedef typename get_binding<ENV, lisp_symbol<SYM> >::value value;
	};
};

// nil
template <> struct analyze<nil>
{
	template <typename ENV> struct eval: pureval<ENV>
	{
		typedef nil value;
	};
};

// (cons REST)
template <typename CAR, typename CDR>
class analyze<cons<CONS, cons<CAR, cons<CDR, nil> > > >
{
	typedef analyze<CAR> fir;
	typedef analyze<CDR> sec;
public:
	template <typename ENV> struct eval
	{
		typedef typename fir::template eval<ENV> efir;
		typedef typename sec::template eval<typename efir::env> esec;
		typedef alloc<typename esec::env, cons<typename efir::value, typename esec::value> > alloced;

		typedef typename alloced::value value;
		typedef typename alloced::env env;
	};
};

template <typename ENV, typename V> struct car;
template <typename ENV, typename CAR, typename CDR>
struct car<ENV, cons<CAR,CDR> >
{
	typedef CAR value;
};
template <typename ENV, typename P> struct car
{
	typedef typename car<ENV, typename peek<ENV, P>::value>::value value;
};

template <typename ENV, typename V> struct cdr;
template <typename ENV, typename CAR, typename CDR>
struct cdr<ENV, cons<CAR,CDR> >
{
	typedef CDR value;
};
template <typename ENV, typename P> struct cdr
{
	typedef typename cdr<ENV, typename peek<ENV, P>::value>::value value;
};

// (car ARG)
template <typename ARG>
struct apply<CAR, cons<ARG, nil> >
{
	template <typename ENV> struct eval
	{
		typedef typename ARG::template eval<ENV> arg;
		typedef typename arg::env env;
		typedef typename car<env, typename arg::value>::value value;
	};
};

// (cdr ARG)
template <typename ARG>
struct apply<CDR, cons<ARG, nil> >
{
	template <typename ENV> struct eval
	{
		typedef typename ARG::template eval<ENV> arg;
		typedef typename arg::env env;
		typedef typename cdr<env, typename arg::value>::value value;
	};
};

// (null ARG)
template <typename ARG>
struct apply<null, cons<ARG, nil> >
{
	template <typename ENV> struct eval
	{
		typedef typename ARG::template eval<ENV> e;
		static const bool val = same_type<typename e::value, nil>::value;
		typedef typename select_type<val, T, nil>::type value;
		typedef typename e::env env;
	};
};

// (quote REST)
template <typename REST>
struct analyze<cons<QUOTE, cons<REST, nil> > >
{
	template <typename ENV> struct eval: pureval<ENV>
	{
		typedef REST value;
	};
};

// (if TEST TRUE_CLAUSE FALSE_CLAUSE)
template <typename TEST, typename T, typename F>
struct analyze<cons<IF, cons<TEST, cons<T, cons<F, nil> > > > >
{
	typedef analyze<TEST> aTest;
	typedef analyze<T> aT;
	typedef analyze<F> aF;
	template <typename ENV> struct eval
	{
		typedef typename aTest::template eval<ENV> test_result;
		typedef typename test_result::env test_env;
		typedef typename select_type<
			same_type<typename test_result::value, nil>::value,
			typename aF::template eval<test_env>,
			typename aT::template eval<test_env> >::type result;
		typedef typename result::value value;
		typedef typename result::env env;
	};
};

// (set (cdr EXPR) FORM)
template <typename EXPR, typename FORM>
struct analyze<cons<SET,cons<cons<CDR,cons<EXPR,nil> >,cons<FORM,nil> > > >
{
	typedef analyze<EXPR> aExpr;
	typedef analyze<FORM> aForm;

	template <typename ENV> struct eval
	{
		typedef typename aExpr::template eval<ENV> expr;
		typedef typename aForm::template eval<typename expr::env> form;

		typedef typename expr::value p;
		typedef typename form::env oldenv;
		typedef typename peek<oldenv, p>::value oldcons;
		typedef cons<typename oldcons::car, typename form::value> value;
		typedef typename poke<oldenv, p, value>::value env;
	};
};

// (set VAR FORM)
template <typename VAR, typename FORM>
struct analyze<cons<SET, cons<VAR, cons<FORM, nil> > > >
{
	typedef analyze<FORM> val_analyze;
	template <typename ENV> struct eval
	{
		typedef typename val_analyze::template eval<ENV> result;
		typedef typename result::value value;
		typedef typename set_binding<VAR, value, typename result::env>::value env;
	};
};

// (define (fun args...) BODY)
template <typename FUN, typename ARGS, typename BODY>
struct analyze<cons<DEFINE, cons<cons<FUN, ARGS>, BODY> > >
{
	typedef analyze<cons<LAMBDA, cons<ARGS, BODY> > > val_analyze;
	template <typename ENV> struct eval
	{
		typedef typename val_analyze::template eval<ENV> result;
		typedef typename result::value value;
		typedef typename set_binding<FUN, value, typename result::env>::value env;
	};
};

// (define VAR FORM)
template <const char* NAME, typename FORM>
struct analyze<cons<DEFINE, cons<lisp_symbol<NAME>, cons<FORM, nil> > > >
{
	typedef lisp_symbol<NAME> VAR;
	typedef analyze<FORM> val_analyze;
	template <typename ENV> struct eval
	{
		typedef typename val_analyze::template eval<ENV> result;
		typedef typename result::value value;
		typedef typename set_binding<VAR, value, typename result::env>::value env;
	};
};

// (progn FIRST REST...)
template <typename FIRST, typename REST>
struct analyze<cons<PROGN, cons<FIRST,REST> > >
{
	typedef analyze<FIRST> first_analyze;
	typedef analyze<cons<PROGN,REST> > rest_analyze;
public:
	template <typename ENV> struct eval
	{
		typedef typename first_analyze::template eval<ENV> first_eval;
		typedef typename rest_analyze::template eval<typename first_eval::env> rest_eval;
		typedef typename rest_eval::value value;
		typedef typename rest_eval::env env;
	};
};
// (progn LAST)
template <typename LAST>
class analyze<cons<PROGN, cons<LAST, nil> > >
{
	typedef analyze<LAST> last_analyze;
public:
	template <typename ENV> struct eval
	{
		typedef typename last_analyze::template eval<ENV> last_eval;
		typedef typename last_eval::value value;
		typedef typename last_eval::env env;
	};
};

template <typename FORMALS, typename ACTUALS, typename ENV>
class create_frame_eval
{
	typedef typename ACTUALS::car::template eval<ENV> result;
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

template <typename FORMALS, typename BODY, typename LEXSP, typename ACTUALS>
struct apply<lambda<FORMALS, BODY, LEXSP>, ACTUALS>
{
	template <typename ENV> struct eval;
	template <typename H, typename SP> struct eval<env_<H,SP> >
	{
		typedef env_<H,SP> ENV;
		typedef env_<H,LEXSP> LEXENV;
		typedef typename create_frame_eval<FORMALS, ACTUALS, ENV>::value new_frame;
		typedef typename push_frame<new_frame, LEXENV>::value subenv;
		typedef typename BODY::template eval<subenv> result;
	public:
		typedef typename result::value value;
		typedef typename re_sp<typename result::env,SP>::value env;
	};
};

template <typename ACTUALS>
struct apply<PLUS, ACTUALS>
{
	template <typename ENV> struct eval
	{
		typedef typename ACTUALS::car::template eval<ENV> result_head;
		typedef typename apply<PLUS, typename ACTUALS::cdr>::template eval<typename result_head::env> result_tail;

		template<typename A, typename B>
		struct plus;

		template<int a, int b>
		struct plus<INT(a), INT(b)>
		{
			typedef INT(a+b) value;
		};
		typedef typename plus<typename result_head::value, typename result_tail::value>::value value;
		typedef typename result_tail::env env;
	};
};

template <>
struct apply<PLUS, nil>
{
	template <typename ENV> struct eval: pureval<ENV>
	{
		typedef INT(0) value;
	};
};

// (lambda ...)
template <typename ARGS, typename BODY>
class analyze<cons<LAMBDA, cons<ARGS, BODY> > >
{
public:
	typedef analyze<cons<PROGN, BODY> > body_analyze;
	template <typename ENV> struct eval: pureval<ENV>
	{
		typedef lambda<ARGS, body_analyze, typename ENV::sp> value;
	};
};

// (FUN ACTUALS)
template <typename FUN, typename ACTUALS>
struct analyze<cons<FUN, ACTUALS> >
{
	typedef typename analyze_many<cons<FUN, ACTUALS> >::value analyzed;
	typedef typename analyzed::car analyzed_fun;
	typedef typename analyzed::cdr analyzed_args;
	template <typename ENV> struct eval
	{
		typedef typename analyzed_fun::template eval<ENV> fun;
		typedef typename apply<typename fun::value, analyzed_args>::template eval<typename fun::env> result;
		typedef typename result::value value;
		typedef typename result::env env;
	};
};

typedef env_<heap<>,nil> really_empty_env;
typedef push_frame<nil, really_empty_env>::value env0;
#define reg_prim(prim, e, e2) \
	typedef add_binding<prim, prim, e>::value e2
reg_prim(PLUS, env0, env1);
reg_prim(CAR, env1, env2);
reg_prim(CDR, env2, env3);
reg_prim(null, env3, env4);
typedef env4 initial_env;

template <typename EXPR, typename ENV>
struct eval
{
	typedef typename analyze<EXPR>::template eval<ENV> evaled;
	typedef typename evaled::value value;
	typedef typename evaled::env env;
};
