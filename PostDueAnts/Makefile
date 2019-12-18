CC = g++
CFLAGS = -Wall --std=c++11
OBJECTS = ants2.o

ants2: $(OBJECTS) #OBJECTS refers to this
	$(CC) $(CFLAGS) -o ants2 $(OBJECTS) -lsfml-graphics -lsfml-window -lsfml-system

ants2.o: ants2.cpp #needs space after colon
	$(CC) $(CFLAGS) -c ants2.cpp -o ants2.o

clean:
	rm ants2 $(OBJECTS)