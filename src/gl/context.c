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

#include "context.h"

#include "surface.h"

#include "../log.h"

#include <memory.h>
#include <stdlib.h>

bool GL_context_initialize(GL_Context_t *gl, size_t width, size_t height)
{
    void *vram = malloc(width * height * sizeof(GL_Color_t));
    if (!vram) {
        Log_write(LOG_LEVELS_ERROR, "<GL> can't allocate VRAM buffer");
        return false;
    }

    void **vram_rows = malloc(height * sizeof(void *));
    if (!vram_rows) {
        Log_write(LOG_LEVELS_ERROR, "<GL> can't allocate VRAM LUT");
        free(vram);
        return false;
    }
    for (size_t i = 0; i < height; ++i) {
        vram_rows[i] = (GL_Color_t *)vram + (width * i);
    }

    Log_write(LOG_LEVELS_DEBUG, "<GL> VRAM allocated at #%p (%dx%d)", vram, width, height);

    *gl = (GL_Context_t){
            .width = width,
            .height = height,
            .vram = vram,
            .vram_rows = vram_rows,
            .vram_size = width * height,
        };

    gl->background = 0;
    for (size_t i = 0; i < GL_MAX_PALETTE_COLORS; ++i) {
        gl->shifting[i] = i;
        gl->transparent[i] = GL_BOOL_FALSE;
    }
    gl->transparent[0] = GL_BOOL_TRUE;

    GL_palette_greyscale(&gl->palette, GL_MAX_PALETTE_COLORS);
    Log_write(LOG_LEVELS_DEBUG, "<GL> calculating greyscale palette of #%d entries", GL_MAX_PALETTE_COLORS);

    return true;
}

void GL_context_terminate(GL_Context_t *gl)
{
    free(gl->vram);
    free(gl->vram_rows);

    *gl = (GL_Context_t){};
}

void GL_context_push(GL_Context_t *gl)
{
}

void GL_context_pop(GL_Context_t *gl)
{
}

void GL_context_clear(GL_Context_t *gl)
{
    const GL_Color_t color = gl->palette.colors[gl->background];
    GL_Color_t *dst = (GL_Color_t *)gl->vram;
    for (size_t i = gl->vram_size; i > 0; --i) {
        *(dst++) = color;
    }
}

void GL_context_blit(const GL_Context_t *context, const GL_Surface_t *surface, GL_Rectangle_t tile, GL_Point_t position, float scale, float rotation)
{
    // TODO: specifies `const` always? Is pedantic or useful?
    // https://dev.to/fenbf/please-declare-your-variables-as-const
    const GL_Pixel_t *shifting = context->shifting;
    const GL_Bool_t *transparent = context->transparent;
    const GL_Color_t *colors = context->palette.colors;

    const GL_Pixel_t *src = (const GL_Pixel_t *)surface->data_rows[tile.y] + tile.x;
    GL_Color_t *dst = (GL_Color_t *)context->vram_rows[position.y] + position.x;
    // const GL_Pixel_t *src = (const GL_Pixel_t *)surface->data + (tile.y * surface->width) + tile.x;
    // GL_Pixel_t *dst = (GL_Pixel_t *)target->data + (position.y * target->width) + position.x;

    const size_t src_skip = surface->width - tile.width;
    const size_t dst_skip = context->width - tile.width;

    for (size_t i = 0; i < tile.height; ++i) {
        for (size_t j = 0; j < tile.width; ++j) {
            GL_Pixel_t index = shifting[*(src++)];
            if (transparent[index]) {
                dst++;
            } else {
                *(dst++) = colors[index];
            }
        }
        src += src_skip;
        dst += dst_skip;
    }

    // TODO: implement clipping, rotation and scaling.
}

void GL_context_blit_fast(const GL_Context_t *context, const GL_Surface_t *surface, GL_Rectangle_t tile, GL_Point_t position)
{
    const GL_Pixel_t *shifting = context->shifting;
    const GL_Bool_t *transparent = context->transparent;
    const GL_Color_t *colors = context->palette.colors;

    const GL_Pixel_t *src = (const GL_Pixel_t *)surface->data_rows[tile.y] + tile.x;
    GL_Color_t *dst = (GL_Color_t *)context->vram_rows[position.y] + position.x;
    // const GL_Pixel_t *src = (const GL_Pixel_t *)surface->data + (tile.y * surface->width) + tile.x;
    // GL_Pixel_t *dst = (GL_Pixel_t *)target->data + (position.y * target->width) + position.x;

    const size_t src_skip = surface->width - tile.width;
    const size_t dst_skip = context->width - tile.width;

    for (size_t i = 0; i < tile.height; ++i) {
        for (size_t j = 0; j < tile.width; ++j) {
            GL_Pixel_t index = shifting[*(src++)];
            if (transparent[index]) {
                dst++;
            } else {
                *(dst++) = colors[index];
            }
        }
        src += src_skip;
        dst += dst_skip;
    }
}
