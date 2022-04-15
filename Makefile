#      _____                __    ______
#     / ___ \              / /   /___  /
#    / /__/ /___  ____  __/ /_______/ /    ____  ____
#   / _____/ __ \/ __ \/_   _/ __  / /    / __ \/ __ \
#  / /    /  ___/ / / / / /_/ /_/ / /____/  ___/ / / /
# /_/     \____/_/ /_/ /___/\__,_/______/\____/_/ /_/
#
# PentaZen, a Gomoku/Renju playing engine developed by Sun Yuliang.
#

# User settings
debug = no
target = pentazen

# Global settings
CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -static -std=c++17
BINDIR = .\bin

# Target
ifeq ($(target), pattern)
	CPPS = line.cpp main.cpp
	EXE = pattern.exe
	SRCDIR = .\src\pattern
	SRCS = $(addprefix $(SRCDIR)\, $(CPPS))
	OBJS = $(addprefix $(SRCDIR)\, $(notdir $(SRCS:.cpp=.o)))
	TARGET = $(addprefix $(BINDIR)\, $(EXE))
endif
ifeq ($(target), pentazen)
	CPPS = board.cpp main.cpp misc.cpp movegen.cpp protocol.cpp search.cpp thread.cpp tt.cpp
	EXE = pbrain-PentaZen.exe
	SRCDIR = .\src
	SRCS = $(addprefix $(SRCDIR)\, $(CPPS))
	OBJS = $(addprefix $(SRCDIR)\, $(notdir $(SRCS:.cpp=.o)))
	TARGET = $(addprefix $(BINDIR)\, $(EXE))
endif

# Compile flags
ifeq ($(debug), no)
	CXXFLAGS += -DNDEBUG -O3 -fno-rtti -fno-exceptions
else
	CXXFLAGS += -g
endif

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)
	@move $(SRCDIR)\*.o $(BINDIR)

all:
	$(TARGET)

clean:
	@del .\bin\*.exe .\bin\*.o .\src\*.o .\src\pattern\*.o

.PHONY: all clean
