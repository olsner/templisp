#include "lisp.h"
#include <stdio.h>

int main()
{
	// (cons 3 (quote 5 cons 2))
/*	typedef LIST3(CONS,
				INT(3),
				LIST4(QUOTE, INT(5), CONS, INT(2))) prog;*/
	// (if nil (cons 5 2) (quote 2 5))
/*	typedef LIST4(IF, T,
				LIST3(CONS, INT(5), INT(2)),
				LIST3(QUOTE, INT(2), INT(5))) prog;*/
	// (if t (quote 5 2) (quote 2 5))
	/*typedef LIST4(IF, T,
				LIST3(QUOTE, INT(5), INT(2)),
				LIST3(QUOTE, INT(2), INT(5))) prog;*/
	// (set a (set b (quote 5 2)))
/*	typedef LIST3(SET, A,
							LIST3(SET,
								  B,
								  LIST3(QUOTE, INT(5), INT(2)))) prog;*/
	// (progn (set a 5) a)
//	typedef LIST3(PROGN, LIST3(SET, A, INT(5)), A) prog;
	// ((lambda (a b) (cons a b)) 5 2)
	typedef LIST3(LIST3(LAMBDA,
						LIST2(A, B),
						CALL2(CONS, A, B)),
				  INT(5), INT(2)) inline_lambda;
	/* (set append
			(lambda (a b)
			  (if (null a)
			  		b
				(cons (car a) (append (cdr a) b))))) */
	typedef LIST3(SET, APPEND,
			LIST3(LAMBDA, LIST2(A, B),
				LIST4(IF,
					LIST2(null, A),
					B,
					LIST3(CONS,
						LIST2(CAR, A),
						LIST3(CONS,
							LIST2(CDR, A),
							B))))) set_append;
	// (append (quote (nil)) (quote (4 5 6)))
	typedef LIST3(APPEND,
				  LIST2(QUOTE, LIST2(INT(1), INT(2))),
				  LIST2(QUOTE, LIST3(INT(4), INT(5), INT(6)))) use_append;
	// (set a 5)
	typedef CALL2(SET, A, INT(5)) set_a;
	// (set c (lambda (a b) (cons <inline_lamba> (car b))))
	typedef LIST3(SET,
				  C,
				  LIST3(LAMBDA, LIST2(A, B),
				  	LIST3(CONS, inline_lambda, LIST2(CAR, B)))) set_c_lambda;
	// (c 1 (quote (2 3)))
	typedef LIST3(C, INT(1), LIST2(QUOTE, LIST2(INT(7), INT(8)))) run_c;
	//typedef LIST4(PROGN, set_c_lambda, set_a, run_c) prog;
	//typedef LIST3(PROGN, set_append, use_append) prog;
	//typedef CALL2(PROGN, CALL2(SET, A, nil), CALL1(null, A)) prog;
	
	typedef set_append prog;
	//typedef use_append prog2;
	typedef LIST3(APPEND,
				  LIST2(QUOTE, LIST1(INT(1))),
				  LIST2(QUOTE, LIST3(INT(4), INT(5), INT(6)))) prog2;
	typedef eval<prog, initial_env> eval_result1;
	typedef eval<prog2, eval_result1::env> eval_result;
	typedef eval_result::value value;

	typedef LIST3(PLUS, INT(7), INT(3)) plus_7_3;
	typedef eval<plus_7_3, initial_env> plus_7_3_res;

	#define PRINT(_type) { print<_type> printed; printf(#_type ": %s.\n", (char *)printed); }

	PRINT(plus_7_3);
	PRINT(plus_7_3_res::value);
	
	PRINT(prog);
	//PRINT(prog2);
	PRINT(eval_result1::value);
	PRINT(eval_result1::env);
	PRINT(prog2);
	PRINT(eval_result::value);
	PRINT(eval_result::env);
}
