#include "lisp.h"
#include "compile.h"

int main()
{
	typedef PROG prog;
	ob res = eval(prog());
	
	printob(res);
	printf("\n");
}
