CXX = g++
# -O2 lets the compiler remove extra work after the code improvements.
# Use make CXXFLAGS="-Wall -g -O0" to compare algorithms without optimization.
# Source: ChatGPT assistance; https://clang.llvm.org/docs/CommandGuide/clang.html
CXXFLAGS = -Wall -g -O2 -std=c++11
LDLIBS = -lsqlite3

.PHONY: all clean

all: traffic-simulation

traffic-simulation: Sim.cpp Sim_Math.cpp TimeCode.cpp BigInteger.cpp Sim_Math.h TimeCode.h BigInteger.h
	$(CXX) $(CXXFLAGS) Sim.cpp Sim_Math.cpp TimeCode.cpp BigInteger.cpp -o $@ $(LDLIBS)

tests: Tests.cpp Sim_Math.cpp TimeCode.cpp TimeCodeTests.cpp BigInteger.cpp BigIntegerTests.cpp Sim_Math.h TimeCode.h TimeCode_Tests.h BigInteger.h BigInteger_Tests.h
	$(CXX) $(CXXFLAGS) Tests.cpp Sim_Math.cpp TimeCode.cpp TimeCodeTests.cpp BigInteger.cpp BigIntegerTests.cpp -o $@ $(LDLIBS)

clean:
	rm -f tests traffic-simulation
