LIB =
CFLAGS = -O2 -DHAVE_BOOST -fpic -std=gnu++0x -I.. -I/home/aag2k/usr/include

CC = gcc

HEADERS = alg.h blkmem.h cols.h distr.h graph.h \
          libgraphs.h link.h netfactory.h node.h \
          parsers.h stdafx.h writers.h

SRC = alg.cpp cols.cpp distr.cpp graph.cpp link.cpp \
      writers.cpp

OBJ = alg.o cols.o distr.o graph.o link.o writers.o

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

libgraphs.so : $(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LIB)
	rm $(OBJ)
	cp $@ ../so

clean:
	rm -Rf $(OBJ) libgraphs.so *~

