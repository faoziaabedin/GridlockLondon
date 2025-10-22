# Simple Makefile for testing route policy
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I.
SOURCES = core/Node.cpp core/Edge.cpp core/City.cpp core/Agent.cpp core/Preset.cpp core/ShortestPathPolicy.cpp
TEST_SOURCES = test_route_policy_simple.cpp

test_route_policy_simple: $(SOURCES) $(TEST_SOURCES)
	$(CXX) $(CXXFLAGS) -o test_route_policy_simple $(SOURCES) $(TEST_SOURCES)

clean:
	rm -f test_route_policy_simple

.PHONY: clean
