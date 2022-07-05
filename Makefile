swap.exe: objects.o swap.o
	g++ -pthread -O3 swap.o objects.o src\json_reader.o src\json_value.o src\json_writer.o -o swap -Lsrc\lib -lsfml-graphics -lsfml-window -lsfml-system

swap.o: swap.cpp
	g++ -Isrc\include -O3 -c swap.cpp

objects.o: objects.cpp objects.hpp
	g++ -Isrc\include -O3 -c objects.cpp

clean:
	-rm *.o $(objects) swap.exe