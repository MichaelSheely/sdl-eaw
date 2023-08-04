## EAW with SDL

Requirements: Installation of SDL2.

`$ sudo apt-get install libsdl2-dev`

Tested on Debian rodete.

Can compile and run via the following commands:

```
$ gcc -o eaw eaw.cc draw_circle.cc $(sdl2-config --cflags --libs) && ./eaw
```

Some code for circle drawing in SDL2 cribbed from
https://github.com/rtrussell/BBCSDL/blob/master/src/SDL2_gfxPrimitives.c
