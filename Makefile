CXXFLAGS = -std=gnu++17 -fno-rtti

%: %.cc $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)
