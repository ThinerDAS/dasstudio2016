CFLAGS   = -std=c99   -Wall -Wextra 
CXXFLAGS = -std=c++11 -Wall -Wextra 
DBGFLAGS = -g3 -DDEBUG -fsanitize=memory,undefined,integer -fsanitize-recover=all 
RELFLAGS = -O3 -DNDEBUG 

.DEFAULT_GOAL := all

.PHONY: all
all: CFLAGS   += $(RELFLAGS)
all: CXXFLAGS += $(RELFLAGS)
all: mtpg

mtpg: mtpg.o mtstat.o
	$(CXX) -o $@ $^

mtpg.o: mtpg.cpp mtstat.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

mtstat.o: mtstat.cpp mtstat.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f mtpg mtpg.o mtstat.o
