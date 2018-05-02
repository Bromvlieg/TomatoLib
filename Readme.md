# Better than brom - TomatoLib

XNA-inspired rendering engine built on OpenGL and SFML.

## Building

### On Windows

Magic.

### On Linux

You need at least: (commands are for Debian distributions)

* CMake 3.0 `apt-get install cmake`
* libglew `apt-get install libglew-dev`
* libfreetype6 `apt-get install libfreetype6-dev`
* libgles1-mesa-dev and libgles2-mesa-dev `apt-get install libgles1-mesa-dev libgles2-mesa-dev`
* xorg-dev `apt-get install xorg-dev`

Init all git submodules: `git submodule init && git submodule update`

### Things to do

* Instead of using the `tomato` branch in [AngeloG/freetype-gl](https://github.com/AngeloG/freetype-gl), figure out if it's safe to use the original master branch instead (which is *not* our own glfw built from source)
