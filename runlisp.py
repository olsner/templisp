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

def clang(out, prog):
	return 'clang++ -o %s "-DPROG=%s" -std=c++0x templ_lisp.cpp' % (out, prog)
def gcc(out, prog):
	return 'g++ -o %s "-DPROG=%s" -ftemplate-depth-30 -std=c++0x -Wall -g templ_lisp.cpp 2>&1 | ./filter.sh' % (out, prog)

args = sys.argv[1:]
if not args:
	print >>sys.stderr, "Usage: %s [--clang|--gcc] LISP..." % sys.argv[0]
	sys.exit(1)
elif args[0] == '--clang':
	args = args[1:]
	compiler = clang
else:
	if args[0] == '--gcc':
		args = args[1:]
	compiler = gcc

for a in args:
	s,rest = parse(a)
	assert not len(rest), "Unconsumed input: %r" % (rest)
	r = os.system('out=`mktemp`; (%s && $out); res=$?; rm -f $out; exit $res' % compiler('$out', p(s)))
	if r: sys.exit(1)
