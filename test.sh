testing2() {
	echo Testing "$1"
	time shelltest -cad --with "$2" tests -- -j4 || exit 1
}

testing() {
	testing2 "$1" "./runlisp.py $2"
}

if true; then
	testing GCC --gcc
	testing clang --clang
	testing 'compiler (gcc)' "--compile --gcc"
else
# Not ready yet
	echo "Compiling mceval..."
	./mceval.sh -b || exit 1
	testing2 'mceval (compiled w/ gcc)' "./mceval.sh -n"
fi
