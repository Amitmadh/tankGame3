# Simulator/Makefile

# === CONFIGURATION ===
ID1 = 322996059
ID2 = 211779582
OUT_EXE = simulator_$(ID1)_$(ID2)

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -fPIC -I../common -I../UserCommon
LDFLAGS = -rdynamic
LDLIBS = -ldl  # Needed for dlopen

# === SOURCE STRUCTURE ===
SRCDIR = .
USERCOMMONDIR = ../UserCommon

SIMULATOR_CPP = $(wildcard $(SRCDIR)/*.cpp)
USERCOMMON_CPP = $(wildcard $(USERCOMMONDIR)/*.cpp)

OBJS = $(SIMULATOR_CPP:.cpp=.o) $(USERCOMMON_CPP:.cpp=.o)

# === BUILD RULES ===

all: $(OUT_EXE)

$(OUT_EXE): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@
	@echo "Cleaning up .o files..."
	@rm -f $(SRCDIR)/*.o $(USERCOMMONDIR)/*.o

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(USERCOMMONDIR)/*.o $(OUT_EXE)

.PHONY: all clean
