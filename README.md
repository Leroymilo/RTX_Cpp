# RTX_Cpp
A raytracing engine made in C++

### To build the main app from source you need to :
- install MSYS2 and mingw
- install sfml, jsoncpp and make using mingw
- add msys2_64\mingw64\bin and msys2_64\usr\bin to your path

If all these conditions are met, you should be able to open a command prompt and type `make` in the project directory to build it (unless I'm still not understanding how it works, which is very likely so please report to me if these informations are false).

### To build the python video builder you'll need :
- python 3.10.x

Install required python libraries with `pip install -r requirements.txt` (or `pip3...` on Linux) in a command prompt in the project directory.

On Windows you can type `auto-py-to-exe` to launch the executable builder and follow the instructions.</br>
On other platforms, you should be able to use pyinstaller (`pyinstaller video_builder.py --onefile`) if you want an app</br>
but the default window size and other stuff might be broken on Linux (I had some issues with wxPython on Linux),</br>
so I'd recommend just typing `python video_builder.py` (or `python3...` on Linux)
