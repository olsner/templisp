#!/usr/bin/env python

special = {
	'+' : 'PLUS',
	'null?' : 'null',
	'set!' : 'SET',
	'number?' : 'NUMBER',
	'eq?' : 'EQ',
	'string?' : 'STRING',
	'symbol?' : 'SYMBOL',
	'pair?' : 'PAIR',
}
presymbols = "cons quote define set! lambda progn if t car cdr + cond let apply list eq? display putc getc null? number? string? symbol? pair?".split()

def cppSafe(sym):
	res = ''
	for c in sym:
		if (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z'):
			res += c
		else:
			res += 'x%d'%ord(c)
	return res

def to_chars(s):
	for c in s:
		yield "'%c'" % c

print "/* GENERATED BY gensyms.py */"
for sym in presymbols:
	cpp = special.get(sym, cppSafe(sym).upper())
	print 'typedef symbol<%s> %s;' % (','.join(to_chars(sym)), cpp)