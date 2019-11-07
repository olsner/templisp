# Template meta-programming Lisp (Scheme)

Lisp S-expressions are encoded into template applications, then interpreted and
evaluated into new template types, or compiled into C++ code with a minimal
(and maximally stupid) run-time system.

The compiler does not do any optimizations yet.

## Dealing with error messages

Template errors are notoriously difficult to read. To translate the template
types back into S-expressions, run your compiler output through `filter.sh`.

Note: the filter code is fairly old now, it needs some updating with recent
compilers.

## Evaluating expressions inline

`runlisp.py` converts S-expressions from the command line into template
typedefs and inserts them into the interpreter or compiler. See `./runlisp.py
--help` for usage and more flags.

For smaller expressions (up to maybe 1k characters) the parsing can be done
with templates too, minimizing the cheating done by through Python - use the
`--compile-time-parsing` flag to activate it.

The metacircular evaluator is too big to be parsed with ~16GB of RAM though,
and if you're experimenting with compile-time parsing larger expressions you
probably want to use something like `ulimit -Sv $[4*1024*1024]` to limit
virtual memory allowed by the compiler to 4GB (adjust limit to taste).

## Compiling and running the metacircular evaluator from SICP

`mceval.sh` wraps `runlisp.py --compile` to compile and launch the metacircular
evaluator. It'll take a while to compile on first run, and will then print `;;;
M-Eval` when it is ready for input.

## Running tests

The test suite is based on shelltestrunner, install shelltest through Cabal or
your distribution package manager, then run `./test.sh` to run the test suite
against GCC and clang, compiler and interpreter and metacircular evaluator.
