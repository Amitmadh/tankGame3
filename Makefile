# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -g

# Directories
OBJDIR = bin
SRCDIR = .
TARGET = tanks_game

# Source files
SRCS = MyTankAlgorithmFactory.cpp MyPlayerFactory.cpp MySatelliteView.cpp MyBattleInfo.cpp MyBattleInfo1.cpp MyBattleInfo2.cpp MyPlayer.cpp \
	   MyTankAlgorithm.cpp Player1.cpp Player2.cpp TankAlgorithm1.cpp TankAlgorithm2.cpp \
	   GameManager.cpp GameObject.cpp Tank.cpp Shell.cpp Mine.cpp Wall.cpp Main.cpp GameUtilis.cpp

# Object files path in bin/
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)
OBJS := $(OBJS:common/%.cpp=$(OBJDIR)/common/%.o)

# Default target
all: $(TARGET)

# Build target executable from object files
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Compile rule for files in root
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile rule for files in common/
$(OBJDIR)/common/%.o: common/%.cpp | $(OBJDIR)/common
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create necessary bin folders
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/common:
	mkdir -p $(OBJDIR)/common

# Clean up
clean:
	rm -rf $(OBJDIR) $(TARGET)