CC  = clang
CXX = clang++

CFLAGS   = -std=c99   -Wall -Wextra -Izlib
CXXFLAGS = -std=c++11 -Wall -Wextra -Izlib
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
all: edict.o

.PHONY: debug
debug: CFLAGS   += $(DBGFLAGS)
debug: CXXFLAGS += $(DBGFLAGS)
debug: LINKER_FLAGS += -lubsan
debug: edict.o

edict.o: edict_s.o base64.o zlibext.o zlib/libz.a.linux
	ld -r -o $@ $(LINKER_FLAGS) $^

edict_s.o: edict.cpp edict.h base64.h zlibext.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

base64.o: base64.c base64.h
	$(CC) $(CFLAGS) -c -o $@ $<

zlibext.o: zlibext.cpp zlibext.h
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#assets.tar.gz: assets
#	GZIP=-9 tar cvfz $@ assets

.PHONY: clean
clean:
	rm -f edict.o edict_s.o base64.o zlibext.o

