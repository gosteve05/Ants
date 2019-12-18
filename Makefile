CC = g++
CFLAGS = -Wall --std=c++11
OBJECTS = ants.o

ants: $(OBJECTS) #OBJECTS refers to this
	$(CC) $(CFLAGS) -o ants $(OBJECTS) -lsfml-graphics -lsfml-window -lsfml-system

ants.o: ants.cpp #needs space after colon
	$(CC) $(CFLAGS) -c ants.cpp -o ants.o

clean:
	rm ants $(OBJECTS)