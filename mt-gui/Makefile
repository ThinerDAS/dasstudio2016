CC  = clang
CXX = clang++

CFLAGS   = -std=c99   -Wall -Wextra 
CXXFLAGS = -std=c++11 -Wall -Wextra 
DBGFLAGS = -g3 -DDEBUG -fsanitize=undefined,integer -fsanitize-recover=all 
RELFLAGS = -O3 -DNDEBUG #-g3

LINKER_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

#EXEC = calc
#OBJS = $(EXEC).o

#DEPENDENCIES = main.cpp benchmk.h bools.h dbgkit.h dbgline.h
#MAINCPP      = main.cpp

.DEFAULT_GOAL := all

.PHONY: all
all: CFLAGS   += $(RELFLAGS)
all: CXXFLAGS += $(RELFLAGS)
all: magictower

.PHONY: debug
debug: CFLAGS   += $(DBGFLAGS)
debug: CXXFLAGS += $(DBGFLAGS)
debug: LINKER_FLAGS += -lubsan
debug: magictower

magictower: mtgui_launcher.o mtstat.o mtgui.o assets.o encodeabledict/edict.o graph.o event_vector.o
	$(CXX) -o $@ $^ $(LINKER_FLAGS)

mtgui_launcher.o: mtgui_launcher.cpp mtstat.h mtgui.h assets.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

mtstat.o: mtstat.cpp mtstat.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

mtgui.o: mtgui.cpp mtgui.h mtstat.h assets.h graph/graph.h graph/event_vector.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

assets.o: assets.cpp assets.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

assets.h: assets/* ash_genh.sh
	./ash_genh.sh > $@

assets.cpp: assets/* ash_gencpp.sh
	./ash_gencpp.sh > $@

graph.o: graph/graph.cpp graph/graph.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

event_vector.o: graph/event_vector.cpp graph/event_vector.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

encodeabledict/edict.o:
	cd encodeabledict; make

#assets.tar.gz: assets
#	GZIP=-9 tar cvfz $@ assets

.PHONY: clean
clean:
	rm -f assets.h assets.cpp assets.o mtstat.o mtgui.o mtgui_launcher.o magictower graph.o event_vector.o

