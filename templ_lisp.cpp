#include "lisp.h"
#include <stdio.h>

int main()
{
	typedef PROG prog;
	typedef eval<prog, initial_env> eval_result;
	print_env<eval_result::env, eval_result::value> printed;
	puts((char*)printed);
}
