#!/usr/bin/env python

import sys
import os
import argparse

special = {
	# TODO Should be handled by adding a mapping from the symbol nil to the special nil value instead
	'nil' : 'nil',
	'null' : 'null',
	'set' : 'SET',
}

def isSymChar(c, first):
	return c in '+-/*?<>!=' or (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') \
		or (not first and c >= '0' and c <= '9')

def cppSafe(sym):
	res = ''
	for c in sym:
		if (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z'):
			res += c
		else:
			res += 'x%d'%ord(c)
	return res

def parseSym(sexp):
	i = 1
	while i < len(sexp) and isSymChar(sexp[i], False):
		i += 1
	return (sexp[:i],), sexp[i:]

def strip(sexp):
	sexp = sexp.strip()
	if sexp[0] == ';':
		i = sexp.find('\n')
		return strip(sexp[i:])
	return sexp

def parseList(sexp, allowUnTerm = False):
	res = []
	while sexp:
		sexp = strip(sexp)
		if sexp[0] == ')': return res, sexp[1:]
		x, sexp = parse(sexp)
		res.append(x)
	assert allowUnTerm, "Unterminated list after %r" % x
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
	sexp = strip(sexp)
	tail = sexp[1:]
	h = sexp[0]
	if h == '(':
		return parseList(tail)
	elif h == '"':
		return tail.split('"',1)
	elif h == "'":
		s,rest = parse(tail)
		return [('quote',), s], rest
	elif isSymChar(h, True):
		return parseSym(sexp)
	else:
		try:
			int(h, 10)
			return parseInt(sexp)
		except:
			pass
	assert False, "Unexpected character %r at start of %r" % (h, sexp)

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
	elif t is str:
		return 'string<%s>' % ','.join(to_chars(s))
	elif t is tuple:
		s = s[0]
		if s in special:
			return special.get(s)
		else:
			return 'symbol<%s>' % ','.join(to_chars(s))

def to_chars(s):
	for c in s:
		if c == '\'': c = "\\'"
		yield "'%s'" % c

clang = 'clang++ -Os -g -std=c++11 -o %s "-DPROG=%s" %s 2>&1'
gcc = 'g++ -Os -g -fmessage-length=0 -ftemplate-depth-1000 -std=c++11 -Wall -Wno-unused-variable -Wno-unused-function -o %s "-DPROG=%s" %s 2>&1 | ./filter.sh'

def mktemp():
	import tempfile
	h = tempfile.NamedTemporaryFile(delete = False)
	name = h.name
	h.close()
	return name

def run(args, prog):
	prog='typedef '+prog+' prog;'
	if args.output is None:
		out = mktemp()
		temp = out
	else:
		out = args.output
		temp = None
	cmd = args.compiler % (out, prog, args.shell)
	try:
		r = os.system(cmd)
		if r: return r
		if args.compile_only:
			return 0
		else:
			return os.system(out)
	finally:
		if temp: os.unlink(temp)
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
parser.add_argument('--compile-only', action='store_true', default=False,
	help="Don't run after compiling")

parser.add_argument('--output', '-o', default=None,
	help="Save compiled executable to given path (default: compile to a temporary file, remove it afterwards)")

args = parser.parse_args()

compiler = gcc

for a in args.expressions:
	s,rest = parse(a)
	assert not len(rest), "Unconsumed input: %r" % (rest)
	if args.action(args, p(s)): sys.exit(1)
