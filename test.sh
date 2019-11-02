testing2() {
	echo "Testing $1 -- $2"
	time shelltest -cad --with "$2" tests -- -j4 || exit 1
}

testing() {
	testing2 "$1" "./runlisp.py $2"
}

(./mceval.sh -b) &

testing GCC --gcc
testing clang --clang
testing 'compiler (gcc)' "--compile --gcc"
wait || exit 1
testing2 'mceval (compiled w/ gcc)' "./mceval.sh -n"
