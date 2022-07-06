rtx.exe: base.o objects.o rtx.o
	g++ -pthread -O3 rtx.o base.o objects.o src\json_reader.o src\json_value.o src\json_writer.o -o rtx -Lsrc\lib -lsfml-graphics -lsfml-window -lsfml-system

rtx.o: rtx.cpp
	g++ -Isrc\include -O3 -c rtx.cpp

objects.o: objects.cpp objects.hpp
	g++ -Isrc\include -O3 -c objects.cpp

base.o: base.cpp base.hpp
	g++ -Isrc\include -O3 -c base.cpp

clean:
	-rm *.o $(objects) rtx.exe