# Template meta-programming Lisp (Scheme)

Lisp S-expressions are encoded into template applications, then interpreted and
evaluated into new template types, or compiled into C++ code with a minimal
(and maximally stupid) run-time system.

## Dealing with error messages

Template errors are notoriously difficult to read. To translate STL types back
into S-expressions, run your compiler output through `filter.sh`.

## Evaluating expressions inline

`runlisp.py` converts incoming S-expressions into template typedefs and inserts
them into the interpreter or compiler. See `./runlisp.py --help` for usage and
more flags.

## Compiling and running the metacircular evaluator from SICP

`mceval.sh` wraps `runlisp --compile` to compile and launch the metacircular
evaluator. It'll take a while to compile on first run, and will then print `;;;
M-Eval` when it is ready for input.

## Running tests

The test suite is based on shelltestrunner, install shelltest through Cabal or
your distribution package manager, then `shelltest -c tests -- -j4`.
