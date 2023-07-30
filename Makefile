СXX?= g++
CXXFLAGS?= -Wall -std=c++2a -g
LDFLAGS=
SOURCES= Graph.cpp Graph.hpp Generate.hpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=graph

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(CXXFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

clean:
	rm -rf *.o *.exe
