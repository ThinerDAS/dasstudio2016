ver=release

CC=clang
CXX=clang++

CFLAGS   = -std=c99   -Wall -Wextra 
CXXFLAGS = -std=gnu++14 -Wall -Wextra 
DBGFLAGS = -Og -g3 -DDEBUG #-fsanitize=memory,undefined,integer -fsanitize-recover=all 
RELFLAGS = -O3 -g3 #-DNDEBUG 
LINKER_FLAGS=

ifeq ($(ver), debug)
CC = gcc
CXX = g++
CFLAGS   += $(DBGFLAGS)
CXXFLAGS += $(DBGFLAGS)
LINKER_FLAGS += $(DBGFLAGS)
else
CFLAGS   += $(RELFLAGS)
CXXFLAGS += $(RELFLAGS)
endif

.DEFAULT_GOAL := all

.PHONY: all
#all: CFLAGS   += $(RELFLAGS)
#all: CXXFLAGS += $(RELFLAGS)
all: main mtg_tester

.PHONY: debug
#debug: CFLAGS   += $(DBGFLAGS)
#debug: CXXFLAGS += $(DBGFLAGS)
debug: main

# There is no header dependency added, so do not use this as final make file TODO

main: main.o ai_core.o ev.o ginterface.o graph.o mtstat.o gsheet.o
	$(CXX) -o $@ $^ $(LINKER_FLAGS)

mtg_tester: mtg_tester.o mtstat.o map_template_generator.o enemy_generator.o gtemplate.o gsheet.o gsheet_filler.o ginterface.o ai_core.o graph.o ev.o
	$(CXX) -o $@ $^

mtg_tester.o: mtg_tester.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

map_template_generator.o: mtai/map_template_generator.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

enemy_generator.o: mtai/enemy_generator.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

gtemplate.o: mtai/gtemplate.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

gsheet.o: mtai/gsheet.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

gsheet_filler.o: mtai/gsheet_filler.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ai_core.o: mtai/ai_core.cpp mtai/ai_core.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ev.o: mtai/event_vector.cpp mtai/event_vector.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

ginterface.o: mtai/ginterface.cpp mtai/ginterface.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

graph.o: mtai/graph.cpp mtai/graph.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

mtstat.o: mtai/mtstat.cpp mtai/mtstat.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f main main.o ai_core.o ev.o ginterface.o graph.o mtstat.o mtg_tester mtg_tester.o mtstat.o map_template_generator.o enemy_generator.o gtemplate.o gsheet.o gsheet_filler.o
