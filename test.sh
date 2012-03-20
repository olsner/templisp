testing() {
	echo Testing "$1"
	time shelltest -cad --with "./runlisp.py $2" tests -- -j4 || exit 1
}

testing GCC --gcc
testing clang --clang
testing 'compiler (gcc)' "--compile --gcc"
