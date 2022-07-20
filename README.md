# RTX_Cpp
A raytracing engine made in C++

### To build the main app from source you need to :
- install MSYS2 and mingw
- install sfml, jsoncpp and make using mingw
- add msys2_64\mingw64\bin and msys2_64\usr\bin to your path

If all these conditions are met, you should be able to open a command prompt and type `make` in the project directory to build it (unless I'm still not understanding how it works, which is very likely so please report to me if these informations are false).

### To build the python video builder you'll need :
- python 3.10.x

Now you can type `pip install -r requirements.txt` in a command prompt in the project directory to install required libraries.</br>
The executable video_builder will be in the folder "dist" (you can delete folders "build", "dist" and the file video_builder.spec once it's done.)
