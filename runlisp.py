#!/usr/bin/env python

import sys
import os
import argparse

special = {
	'+' : 'PLUS',
	'null' : 'null',
	'set!' : 'SET',
}

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

def concat(*xs):
	res = ''
	for s in xs:
		if res and res[-1] == '>' and s[0] == '>':
			res += ' '
		res += s
	return res

def p(s):
	t = type(s)
	if t is list:
		if len(s):
			return concat('cons<',p(s[0]),',',p(s[1:]),'>')
		else:
			return 'nil'
	elif t is int:
		return 'value_type<int,%d>' % s
	else:
		return special.get(s, s.upper())

clang = 'clang++ -g -o %s "-DPROG=%s" -std=c++0x %s 2>&1'
gcc = 'g++ -g -o %s "-DPROG=%s" -ftemplate-depth-30 -std=c++0x -Wall %s 2>&1 | ./filter.sh'

def run(args, prog):
	cmd = args.compiler % ('$out', prog, args.shell)
	if args.output is None:
		cmd = 'out=`mktemp`; (%s && $out); res=$?; rm -f $out; exit $res' % cmd
	else:
		cmd = 'out=\"%s\"; %s' % (args.output, cmd)
	return os.system(cmd)
def justPrint(args, prog):
	print prog

parser = argparse.ArgumentParser(description="Translate S-expressions to templates and display or compile/interpret using a C++ compiler.")
parser.add_argument('expressions', metavar='SEXP', type=str, nargs='+',
	help='an S-expression to translate/compile/interpret')

parser.add_argument('--clang', dest='compiler', action='store_const',
	const=clang, default=gcc,
	help='use clang as the C++ compiler')
parser.add_argument('--gcc', dest='compiler', action='store_const',
	const=gcc,
	help='use g++ as the C++ compiler')

parser.add_argument('--print', dest='action', action='store_const',
	const=justPrint, default=run,
	help="Don't compile/run, just print the template type corresponding to the S-expressions")
parser.add_argument('--compile', dest='shell', action='store_const',
	const='compile.cpp', default='templ_lisp.cpp',
	help="Compile instead of interpreting")

parser.add_argument('--output', '-o', default=None,
	help="Save compiled executable to given path (default: compile to a temporary file, remove it afterwards)")

args = parser.parse_args()

compiler = gcc

for a in args.expressions:
	s,rest = parse(a)
	assert not len(rest), "Unconsumed input: %r" % (rest)
	if args.action(args, p(s)): sys.exit(1)
