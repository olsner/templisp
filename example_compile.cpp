#define COMPILER
#include "lisp.h"
#include "parse.h"
#include "compile.h"
#include <stdio.h>

using prog = decltype(R"foo((progn (define (append a b) (if (null? a) b (cons (car a) (append (cdr a) b))))  (append '(1 2) '(3 4))))foo"_lisp);

int main()
{
    // eval<> expands the program into a function (at compile time) and calls
    // it with an initial environment populated with primitives (at run time).
    // It returns the value of the expression.
    ob res = eval<prog>();
    printob(res);
    puts("");
}
