#!/usr/bin/env python

import sys
import os

special = { '+' : 'PLUS', 'null' : 'null' }

def isSymChar(c, first = False):
	return c in '+-/*' or (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z')

def parseSym(sexp):
	i = 1
	while i < len(sexp) and isSymChar(sexp[i]):
		i += 1
	return sexp[:i], sexp[i:]

def parseList(sexp, allowUnTerm = False):
	res = []
	while sexp:
		if sexp[0] == ')': return res, sexp[1:]
		x, sexp = parse(sexp)
		res.append(x)
	assert allowUnTerm, "Unterminated list"
	return res, sexp

def parseInt(sexp):
	n = 1
	try:
		while n < len(sexp):
			int(sexp[n], 10)
			n += 1
	except:
		pass

	return int(sexp[:n]), sexp[n:]

def parse(sexp):
	sexp = sexp.strip()
	tail = sexp[1:]
	h = sexp[0]
	if h == '(':
		return parseList(tail)
	elif isSymChar(h, True):
		return parseSym(sexp)
	else:
		int(h, 10)
		return parseInt(sexp)

def p(s):
	t = type(s)
	if t is list:
		if len(s):
			return 'cons<%s,%s> ' % (p(s[0]), p(s[1:]))
		else:
			return 'nil'
	elif t is int:
		return 'value_type<int,%d> ' % s
	else:
		return special.get(s, s.upper())

for a in sys.argv[1:]:
	s,rest = parse(a)
	assert not len(rest), "Unconsumed input: %r" % (rest)
	os.system('out=`mktemp`; clang++ -o $out -O3 "-DPROG=%s" templ_lisp.cpp && ($out; rm $out)' % p(s))
