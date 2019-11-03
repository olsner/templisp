CXXFLAGS = -std=gnu++17 -g

%: %.cc $(wildcard *.h)
