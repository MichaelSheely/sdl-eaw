## EAW with SDL

Requirements: Installation of SDL2 and SDL2image.

`$ sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev`

Tested on Debian rodete.

Can compile and run via the following commands:

```
$ g++ -o eaw eaw.cc draw_circle.cc $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf \
&& ./eaw --assets='/path/to/image/assets' --typeface='/path/to/typeface.ttf'
```

Some code for circle drawing in SDL2 cribbed from
https://github.com/rtrussell/BBCSDL/blob/master/src/SDL2_gfxPrimitives.c
