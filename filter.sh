#!/bin/bash
# vim:et:
# Operates on stdin and stdout only

C_TO_LISP='
s/\(struct \)\?value_type<\([a-z]*\), \([0-9]*\)>/\3/g
s/\([a-z_]*\)</(\1 /g
s/, / /g
s/\([ ]*\)>/)/g
s/`(/(/g
s/)[^)]*$/)/g
:replace_symbol
    s/\(symbol[^) ]*\) '"'"'\(.\)'"'"'\( '"'"'\|)\)/\1\2\3/g
    t replace_symbol
s/(symbol\([^)]*\))/"\1"/g
'

CONS_TO_LIST='
s/[ ]\+\([^ ]\+\)/ \1/g
s/(cons (\([A-Z0-9 ()]\+\)) nil)/((\1))/g # (cons (...) nil) => ((...))
s/(cons \([A-Z0-9]\+\) nil)/(\1)/g # (cons SYM nil) => (SYM)
s/(cons \([A-Z0-9]\+\) (\([A-Z0-9 ()]\+\)))/(\1 \2)/g # (cons SYM (...)) => (SYM ...)
s/(cons (\([A-Z0-9 ()]\+\)) (\([A-Z0-9 ()]\+\)))/((\1) \2)/g # (cons (...) (...)) => ((...) ...)'

sed -e "$C_TO_LISP" -e "$CONS_TO_LIST"
