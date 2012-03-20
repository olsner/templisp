#include "lisp.h"
#include <stdio.h>

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

int main()
{
	typedef PROG prog;
	typedef eval<prog, initial_env> eval_result;
	print_env<eval_result::env, eval_result::value> printed;
	puts((char*)printed);
}
