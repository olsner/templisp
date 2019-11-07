CXXFLAGS = -std=gnu++17 -fno-rtti -Os

%: %.cc $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS) $(LIBS)
