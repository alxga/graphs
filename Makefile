LIB =
CFLAGS = -fpic -std=gnu++0x -I..

ifneq ($(VSCFG), LinuxRelease)
  CFLAGS += -g
else
  CFLAGS += -O2
endif

CC = g++

HEADERS = alg.h bgraph.h blkmem.h bnode.h circgraph.h circnode.h \
          circnodeparser.h circnodewriter.h cols.h graph.h libgraphs.h link.h \
          netfactory.h node.h parsers.h sfdistr.h stdafx.h writers.h

SRC = alg.cpp bgraph.cpp circgraph.cpp circnodewriter.cpp cols.cpp graph.cpp \
      sfdistr.cpp writers.cpp

OBJ = alg.o bgraph.o circgraph.o circnodewriter.o cols.o graph.o \
      sfdistr.o writers.o

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

libgraphs.so : $(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LIB)
#	rm $(OBJ)
	cp $@ ../so

clean:
	rm -Rf $(OBJ) libgraphs.so *~

