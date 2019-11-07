testing2() {
	echo "Testing $1 -- $2"
	time shelltest -cad --with "$2" tests -- -j`nproc`
}

testing() {
	testing2 "$1" "./runlisp.py $2"
}

set -e

# compile-time-parsing should be the default later (mceval doesn't use it though)
testing "GCC" "--gcc --compile-time-parsing"
testing "clang" "--clang --compile-time-parsing"
testing 'compiler (gcc)' "--compile --gcc --compile-time-parsing"

echo "Building mceval..."
./mceval.sh -b
testing2 'mceval (compiled w/ gcc)' "./mceval.sh -n"
