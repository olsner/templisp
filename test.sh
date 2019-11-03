testing2() {
	echo "Testing $1 -- $2"
	time shelltest -cad --with "$2" tests -- -j`nproc` || exit 1
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

testing "GCC (compile time parser)" "--gcc --compile-time-parsing"
testing 'compiler (gcc, CTP)' "--compile --gcc --compile-time-parsing"
testing "clang (compile time parser)" "--clang --compile-time-parsing"
