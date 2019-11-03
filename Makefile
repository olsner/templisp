CXXFLAGS = -std=gnu++17 -g

%: %.cc $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)
