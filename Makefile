CXXFLAGS=-ftemplate-depth-30 -Wall -g

all: templ_lisp

clean:
	$(RM) templ_lisp

templ_lisp: templ_lisp.cpp utils.h print.h lists.h
	g++ $(CXXFLAGS) $< -o $@ 2>&1
