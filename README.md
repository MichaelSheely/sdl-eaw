## EAW with SDL

Requirements: Installation of SDL2.

`$ sudo apt-get install libsdl2-dev`

Tested on Debian rodete.

Can compile and run via the following commands:

```
$ gcc -o eaw eaw.cc $(sdl2-config --cflags --libs) && ./eaw
```
