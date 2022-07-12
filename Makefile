rtx.exe: base.o objects.o rtx.o
	g++ -O3 base.o objects.o rtx.o -o rtx -pthread -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -ljsoncpp -static

rtx.o: rtx.cpp
	g++ -Isrc\include -O3 -c rtx.cpp -DSFML_STATIC -static

objects.o: objects.cpp objects.hpp
	g++ -Isrc\include -O3 -c objects.cpp -DSFML_STATIC -static

base.o: base.cpp base.hpp
	g++ -Isrc\include -O3 -c base.cpp -DSFML_STATIC -static

clean:
	-rm *.o $(objects) rtx.exe