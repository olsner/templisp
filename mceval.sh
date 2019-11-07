#!/bin/bash

r() {
	rlargs=$1
	shift
	./runlisp.py --compile $rlargs "(progn `cat ch4-mceval.scm mceval-wrapper.scm` $@)"
}

set -e

if [ $# = 0 ]; then
	r "-o ./mcrepl" "(driver-loop)"
else
	if [ "$1" = -n ]; then
		shift
	else
		r "-o ./mceval --compile-only" "(read-eval)"
	fi
	if [ "$1" = -b ]; then
		exit 0
	fi
	for a in "$@"; do
		echo "$a" | ./mceval 2>&1
	done
fi
