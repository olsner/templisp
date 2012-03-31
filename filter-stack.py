import sys

for line in sys.stdin:
	while len(line):
		if '(char)' not in line:
			print line
			break
		h,r = line.split('(char)', 1)
		print h,
		i = 0
		while r[i] >= '0' and r[i] <= '9':
			i += 1
		print chr(int(r[:i])),
		line = r[i:]

