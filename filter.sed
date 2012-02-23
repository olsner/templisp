s/lisp_symbol<(&lisp_symbol_text_\([A-Z]*\))>/\1/g
s/value_type<\([a-z]*\), \([0-9]*\)>/\2/g
s/\([a-z_]*\)</(\1 /g
s/, / /g
s/\([ ]*\)>/)/g
