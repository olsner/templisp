out=`mktemp`
clang++ -o $out -O3 templ_lisp.cpp && ($out; rm $out)
