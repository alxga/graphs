LIB =
CFLAGS = -O2 -fpic -std=gnu++0x -I..

CC = g++

HEADERS = alg.h blkmem.h cols.h graph.h libgraphs.h link.h netfactory.h \
          node.h parsers.h sfdistr.h stdafx.h writers.h

SRC = alg.cpp cols.cpp graph.cpp link.cpp sfdistr.cpp writers.cpp

OBJ = alg.o cols.o graph.o link.o sfdistr.o writers.o

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

libgraphs.so : $(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LIB)
	rm $(OBJ)
	cp $@ ../so

clean:
	rm -Rf $(OBJ) libgraphs.so *~

