CXXFLAGS=-ftemplate-depth-30 -Wall -g

all: templ_lisp dp

clean:
	$(RM) templ_lisp
	$(RM) dp dp.o

templ_lisp: templ_lisp.cpp utils.h print.h lists.h
	g++ $(CXXFLAGS) $< -o $@ 2>&1

dp: dp.o
	g++ $(CXXFLAGS) $< -o $@
