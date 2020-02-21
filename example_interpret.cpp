#include "lisp.h"
#include "parse.h"
#include <stdio.h>

// Setup the initial environment type
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

// The input expression is parsed at compile time and encoded in a type.
using prog = decltype(R"foo(
(progn
    (define (append a b) (if (null? a) b (cons (car a) (append (cdr a) b))))
    (append '(1 2) '(3 4))) )foo"_lisp);

int main()
{
    // Evaluate the program type and initial environment type into a resulting
    // environment and value type.
    using eval_result = eval<prog, initial_env>;
    // deep_peek_t replaces all pointers in the resulting expression with their
    // current values on the heap. Note that the heap in this case is also a
    // compile-time value encoded in a type.
    using final_value = deep_peek_t<eval_result::env, eval_result::value>;
    // Finally, printed<T> is a compile-time evaluated character array. If
    // everything works correctly this will compile the whole program into a
    // single call to puts() with a pointer to static data.
    puts(printed<final_value>);
}
