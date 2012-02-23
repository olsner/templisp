CXXFLAGS=-ftemplate-depth-30 -Wall -g

all: slist templ_lisp dp

clean:
	$(RM) slist slist.o
	$(RM) templ_lisp
	$(RM) dp dp.o

slist: slist.o
	g++ $(CXXFLAGS) $< -o $@

templ_lisp: templ_lisp.cpp templ_lists2.h
	g++ $(CXXFLAGS) $< -o $@ 2>&1

dp: dp.o
	g++ $(CXXFLAGS) $< -o $@
