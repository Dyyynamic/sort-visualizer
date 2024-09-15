CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Weffc++ -Wold-style-cast -Woverloaded-virtual -fmax-errors=3 -g
LDFLAGS = -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system

main:
	$(CXX) $(LDFLAGS) main.cpp sorts.cpp -o sort

pedantic:
	$(CXX) $(CXXFLAGS) $(LDFLAGS) main.cpp sorts.cpp -o sort

clean:
	rm -f sort
