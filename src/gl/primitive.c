/*
 * Copyright (c) 2019 Marco Lizza (marco.lizza@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 **/

#include "primitive.h"

#include "gl.h"

static bool GL_is_visible(GL_Context_t *gl, GL_Point_t position)
{
    return true;
}

void GL_primitive_point(GL_Context_t *gl, GL_Point_t position, GL_Pixel_t color)
{
    if (!GL_is_visible(gl, position)) {
        return;
    }

    const GL_Pixel_t index = gl->shifting[color];

    if (gl->transparent[index]) {
        return;
    }

    const GL_Color_t rgba = gl->palette.colors[index];

    GL_Color_t *dst = (GL_Color_t *)gl->vram_rows[position.y] + position.x;
    *dst = rgba;
}

void GL_primitive_line(GL_Context_t *gl, GL_Point_t from, GL_Point_t to, GL_Pixel_t color)
{

}

void GL_primitive_hline(GL_Context_t *gl, GL_Point_t origin, size_t width, GL_Pixel_t color)
{
    if (!GL_is_visible(gl, origin)) {
        return;
    }

    const GL_Pixel_t index = gl->shifting[color];

    if (gl->transparent[index]) {
        return;
    }

    const GL_Color_t rgba = gl->palette.colors[index];

    GL_Color_t *dst = (GL_Color_t *)gl->vram_rows[origin.y] + origin.x;
    for (size_t i = 0; i < width; ++i) {
        *(dst++) = rgba;
    }
}

void GL_primitive_vline(GL_Context_t *gl, GL_Point_t origin, size_t height, GL_Pixel_t color)
{
    if (!GL_is_visible(gl, origin)) {
        return;
    }

    const GL_Pixel_t index = gl->shifting[color];

    if (gl->transparent[index]) {
        return;
    }

    const GL_Color_t rgba = gl->palette.colors[index];

    for (size_t i = 0; i < height; ++i) {
        GL_Color_t *dst = (GL_Color_t *)gl->vram_rows[origin.y] + origin.x;
        *(dst++) = rgba;
    }
}

void GL_primitive_rectangle(GL_Context_t *gl, GL_Rectangle_t rectangle, GL_Pixel_t color)
{

}

void GL_primitive_filled_rectangle(GL_Context_t *gl, GL_Rectangle_t rectangle, GL_Pixel_t color)
{

}

void GL_primitive_circle(GL_Context_t *gl, GL_Point_t center, float radius, GL_Pixel_t color)
{

}

void GL_primitive_filled_circle(GL_Context_t *gl, GL_Point_t center, float radius, GL_Pixel_t color)
{

}
