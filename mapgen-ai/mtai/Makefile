CC  = clang
CXX = clang++

CFLAGS   = -std=c99   -Wall -Wextra 
CXXFLAGS = -std=c++11 -Wall -Wextra 
DBGFLAGS = -g3 -DDEBUG -fsanitize=undefined,integer -fsanitize-recover=all 
RELFLAGS = -O3 -DNDEBUG #-g3

LINKER_FLAGS = #-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#EXEC = calc
#OBJS = $(EXEC).o

#DEPENDENCIES = main.cpp benchmk.h bools.h dbgkit.h dbgline.h
#MAINCPP      = main.cpp

.DEFAULT_GOAL := all

.PHONY: all
all: CFLAGS   += $(RELFLAGS)
all: CXXFLAGS += $(RELFLAGS)
all: mapgen

.PHONY: debug
debug: CFLAGS   += $(DBGFLAGS)
debug: CXXFLAGS += $(DBGFLAGS)
debug: LINKER_FLAGS += -lubsan
debug: mapgen

mapgen: mtstat.o mapgen.o
	$(CXX) -o $@ $^ $(LINKER_FLAGS)

mapgen.o: mapgen.cpp mtstat.h mapgen_engine.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

mtstat.o: mtstat.cpp mtstat.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f mtstat.o mapgen
