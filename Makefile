rtx.exe: base.o objects.o rtx.o
	g++ -O3 rtx.o base.o objects.o -o rtx -pthread -Lsrc\lib -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp -static -static-libgcc -static-libstdc++

rtx.o: rtx.cpp
	g++ -Isrc\include -O3 -c rtx.cpp

objects.o: objects.cpp objects.hpp
	g++ -Isrc\include -O3 -c objects.cpp

base.o: base.cpp base.hpp
	g++ -Isrc\include -O3 -c base.cpp

clean:
	-rm *.o $(objects) rtx.exe