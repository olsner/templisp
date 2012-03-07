#!/bin/bash
# vim:et:
# Operates on stdin and stdout only

COLLAPSE_LINES='
BEGIN {
    out=0
}

/^(templ_lisp|lisp|print|utils|heap)/ {
    printf "\n%s", $0
    out=1
}

$0 !~ /^(templ_lisp|lisp|print|utils|heap)/ {
    printf "%s", $0
    out=1
}

END {
    if (out) print ""
}'

C_TO_LISP='
s/lisp_symbol \?<((const char\*)(& lisp_symbol_text_\([a-zA-Z_][a-zA-Z_0-9]*\)))>/\1/g
s/\(struct \)\?value_type<\([a-z]*\), \([0-9]*\)>/\3/g
s/\([a-z_]*\)</(\1 /g
s/, / /g
s/\([ ]*\)>/)/g
s/`(/(/g
s/)[^)]*$/)/g'

CONS_TO_LIST='
s/[ ]\+\([^ ]\+\)/ \1/g
s/(cons (\([A-Z0-9 ()]\+\)) nil)/((\1))/g # (cons (...) nil) => ((...))
s/(cons \([A-Z0-9]\+\) nil)/(\1)/g # (cons SYM nil) => (SYM)
s/(cons \([A-Z0-9]\+\) (\([A-Z0-9 ()]\+\)))/(\1 \2)/g # (cons SYM (...)) => (SYM ...)
s/(cons (\([A-Z0-9 ()]\+\)) (\([A-Z0-9 ()]\+\)))/((\1) \2)/g # (cons (...) (...)) => ((...) ...)'

grep -v '^make:' | \
awk "$COLLAPSE_LINES" | \
sed -e "$C_TO_LISP $CONS_TO_LIST"
