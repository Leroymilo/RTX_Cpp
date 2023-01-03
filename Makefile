lin: base.o objects.o rtx.o
	g++ -O3 base.o objects.o rtx.o -o rtx.out -pthread -Lsrc/lib -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system -ljsoncpp

rtx.exe: base.o objects.o rtx.o
	g++ -O3 base.o objects.o rtx.o -o rtx -pthread -Lsrc/lib -lsfml-graphics-s -lsfml-window-s -lsfml-system-s -lopengl32 -lwinmm -lgdi32 -ljsoncpp -static

rtx.o: rtx.cpp
	g++ -Isrc/include -O3 -c rtx.cpp -DSFML_STATIC -static

objects.o: objects.cpp
	g++ -Isrc/include -O3 -c objects.cpp -DSFML_STATIC -static

base.o: base.cpp
	g++ -Isrc/include -O3 -c base.cpp -DSFML_STATIC -static

clear:
	-rm *.o $(objects) rtx.exe rtx.out

clear link:
	-rm rtx.out rtx.exe