#include "lisp.h"
#include "compile.h"

PROG

int main()
{
	ob res = eval(prog());
	
	printob(res);
	printf("\n");
}
