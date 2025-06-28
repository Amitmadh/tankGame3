# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -pedantic -g

# Target binary
TARGET = tanks_game

# Directories
OBJDIR = bin
SRCDIRS = . Simulator Algorithm GameManager common UserCommon

# Source files
SRCS := $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

# Object files (mirrored under bin/)
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)

# Default target
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

# Compile .cpp -> .o into mirrored bin/ directory
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -rf $(OBJDIR) $(TARGET)
