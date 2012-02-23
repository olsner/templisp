/^templ_lisp/ {
	printf "\n%s", $0
}

$0 !~ /^templ_lisp/ {
	printf "%s", $0
}
