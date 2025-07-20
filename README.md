## EAW with SDL

Requirements: Installation of SDL2 and SDL2image.

### Debian instructions

`$ sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev`

Tested on Debian rodete.

Can compile and run via the following commands:

```
$ g++ -o eaw eaw.cc draw_circle.cc $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf \
&& ./eaw --assets='/path/to/image/assets' --typeface='/path/to/typeface.ttf'
```

### Windows instructions

For running SDL2 on windows.

Installed MSYS2 for MINGW64 from https://www.msys2.org/.

Ran the following:

```
$ pacman -Syu
$ pacman -Su
$ pacman -S mingw-w64-x86_64-toolchain
$ pacman -S mingw-w64-x86_64-SDL2
$ pacman -S mingw-w64-x86_64-SDL2_image
$ pacman -S mingw-w64-x86_64-SDL2_ttf
```

Debug that basic SDL is working in windows:

```
$ g++ debug.cc -o debug.exe -I "/mingw64/include/SDL2" -lmingw32 -lSDL2main -lSDL2
```

### Attributions

Some code for circle drawing in SDL2 cribbed from
https://github.com/rtrussell/BBCSDL/blob/master/src/SDL2_gfxPrimitives.c
