#define COMPILER
#include "lisp.h"
#include "parse.h"
#include "compile.h"

namespace {

PROG

}

int main()
{
	ob res = eval(prog());
	printob(res);
	printf("\n");
}
