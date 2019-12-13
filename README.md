# Tofu Engine

Welcome!

Guess what? Yup, that's yet another game engine/framework.

## Pros

* Carefully crafted C99 code.
* Self-contained, no external modules/libraries required (system-wide libraries excluded).
* Multi-platform through coss-compilation (Windows and Linux, no MacOS support will ever be provided).

## Dependecies

* [Glad](https://glad.dav1d.de/)
* [GLFW](https://www.glfw.org/) v3.4
* [Lua](https://lua.org/) v5.3.5
* [spleen](https://github.com/fcambus/spleen) fonts
* [stb](https://github.com/nothings/stb) libraries

## Features

* [x] Fully script-based, using Lua.
* [x] Straight multimedia support, no intermediate third-party libraries (OpenGL 2.1 required).
* [x] Windowed/fullscreen support with automatic scaling.
* [x] Palette based graphics with up to 256 colors.
* [x] Internal software renderer.
* [x] Per-color re-indexing (*shifting*) and transparency, affecting drawing operations (global, too?).
* [x] Automatic nearest-color palette indexing of images.
* [x] Predefined library of 8/16/32/64 colors palettes.
* [x] Tiled-map support w/ camera support (zoom and scrolling).
* [x] Out-of-the-box timers support.
* [x] Customizable application icon.
* [ ] Out-of-the-box easing functions (see [this](https://github.com/kikito/tween.lua/blob/master/tween.lua) and [this](https://github.com/rxi/flux/blob/master/flux.lua)).
* [ ] Animation support w/ frameset DSL (i.e. compiling a string where each token can be a single frame, a range or a "keep-current-frame for some time" command). Each frameset can have its one update period, and will be most likely based upon a timer.
* [ ] Out-of-the-box palette switching (with tweening) features.
* [ ] Game state and display transitions (at which level? Engine or script?).
* [ ] **Bit** **Bl**ock **T**ransfer operations when drawing (also, [stencil](https://learnopengl.com/Advanced-OpenGL/Stencil-testing) support, see [this](https://open.gl/depthstencils)).
* [ ] Library of "retro-feel" shaders.
* [ ] Library of noise functions ([cellular](https://thebookofshaders.com/12/), Perlin, etc...).
* [ ] Camera/screen shaking by using a post-processing shader.
* [ ] Digital/analogue game-controller support.
* [ ] Audio support (based upon [dr-soft/miniaudio](https://github.com/dr-soft/miniaudio)) w/ run-time multi-voice synth (a-la [Bfxr](https://www.bfxr.net)).
* [ ] Support for TARed/ZIPed games ([rxi/microtar](https://github.com/rxi/microtar), [kuba--/zip](https://github.com/kuba--/zip)).

## Desiderata

* [ ] Hot-reload of selected resources (fonts, banks, maps, shaders, sounds).
* [ ] Define some fixed resolutions (see [this](https://pacoup.com/2011/06/12/list-of-true-169-resolutions/))?
* [ ] Use a custom memory-management allocator.
* [ ] Switch to [Vulkan API](https://www.khronos.org/vulkan/) (through [GLFW](https://www.glfw.org/)).

## Inspirations

* [Love2D](https://love2d.org/)
* [Pico-8](https://www.lexaloffle.com/pico-8.php)
* [tac08](https://github.com/0xcafed00d/tac08/)
* [picolove](https://github.com/picolove/picolove/)
* [raylib](https://www.raylib.com/)
* [DOME Engine](https://github.com/avivbeeri/dome/)

## License

Copyright (c) 2019 Marco Lizza (marco.lizza@gmail.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
