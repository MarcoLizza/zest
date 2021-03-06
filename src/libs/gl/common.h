/*
 * Copyright (c) 2019-2020 by Marco Lizza (marco.lizza@gmail.com)
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

#ifndef __GL_COMMON_H__
#define __GL_COMMON_H__

#include <stddef.h>
#include <stdint.h>

#include <core/platform.h>

typedef uint8_t GL_Pixel_t;

typedef int8_t GL_Bool_t;

typedef struct _GL_Point_t {
    int x, y;
} GL_Point_t;

typedef struct _GL_Size_t {
    size_t width, height;
} GL_Size_t;

typedef struct _GL_Rectangle_t {
    int x, y;
    size_t width, height;
} GL_Rectangle_t;

typedef struct _GL_Quad_t {
    int x0, y0;
    int x1, y1;
} GL_Quad_t;

typedef struct _GL_Image_t {
    size_t width, height;
    const void *data;
} GL_Image_t;

#pragma pack(push, 1)
typedef struct _GL_Color_t {
#if PLATFORM_ID == PLATFORM_WINDOWS
    uint8_t b, g, r, a;
#else
    uint8_t r, g, b, a;
#endif
} GL_Color_t;
#pragma pack(pop)

#define GL_DEGREES_OVER_RADIANS 57.295779513082320876798154814105

#define GL_BOOL_FALSE   0
#define GL_BOOL_TRUE    1

#endif  /* __GL_COMMON_H__ */