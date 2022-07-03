main.exe: main.o objects.o
	g++ main.o objects.o src\json_reader.o src\json_value.o src\json_writer.o -o main -Lsrc\lib -lsfml-graphics -lsfml-window -lsfml-system
	clear

main.o: main.cpp
	g++ -Isrc\include -c main.cpp

objects.o: objects.cpp objects.hpp
	g++ -Isrc\include -c objects.cpp

clean:
	-rm *.o $(objects) main.exe