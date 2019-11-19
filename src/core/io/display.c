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

#include "display.h"

#include <config.h>
#include <core/engine.h>
#include <libs/log.h>
#include <libs/imath.h>

#include <memory.h>
#include <stdlib.h>
#ifdef DEBUG
  #include <stb/stb_leakcheck.h>
#endif

typedef struct _Program_Data_t {
    const char *vertex_shader;
    const char *fragment_shader;
} Program_Data_t;

typedef enum Uniforms_t {
    UNIFORM_TEXTURE,
    UNIFORM_RESOLUTION,
    UNIFORM_TIME,
    Uniforms_t_CountOf
} Uniforms_t;

#define VERTEX_SHADER \
    "#version 120\n" \
    "\n" \
    "varying vec2 v_texture_coords;\n" \
    "\n" \
    "void main()\n" \
    "{\n" \
    "   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n" \
    "   gl_FrontColor = gl_Color; // Pass the vertex drawing color.\n" \
    "\n" \
    "   v_texture_coords = gl_MultiTexCoord0.st; // Retain texture 2D position.\n" \
    "}\n" \

#define FRAGMENT_SHADER_PASSTHRU \
    "#version 120\n" \
    "\n" \
    "varying vec2 v_texture_coords;\n" \
    "\n" \
    "uniform sampler2D u_texture0;\n" \
    "uniform vec2 u_resolution;\n" \
    "uniform float u_time;\n" \
    "\n" \
    "vec4 passthru(vec4 color, sampler2D texture, vec2 texture_coords, vec2 screen_coords) {\n" \
    "    return texture2D(texture, texture_coords) * color;\n" \
    "}\n" \
    "\n" \
    "void main()\n" \
    "{\n" \
    "    gl_FragColor = passthru(gl_Color, u_texture0, v_texture_coords, gl_FragCoord.xy);\n" \
    "}\n"

#define FRAGMENT_SHADER_CUSTOM \
    "#version 120\n" \
    "\n" \
    "varying vec2 v_texture_coords;\n" \
    "\n" \
    "uniform sampler2D u_texture0;\n" \
    "uniform vec2 u_resolution;\n" \
    "uniform float u_time;\n" \
    "\n" \
    "vec4 effect(vec4 color, sampler2D texture, vec2 texture_coords, vec2 screen_coords);\n" \
    "\n" \
    "void main()\n" \
    "{\n" \
    "    gl_FragColor = effect(gl_Color, u_texture0, v_texture_coords, gl_FragCoord.xy);\n" \
    "}\n" \
    "\n"

static const Program_Data_t _programs_data[Display_Programs_t_CountOf] = {
    { VERTEX_SHADER, FRAGMENT_SHADER_PASSTHRU },
    { NULL, NULL }
};

static const int _texture_id_0 = 0;

static const char *_uniforms[Uniforms_t_CountOf] = {
    "u_texture0",
    "u_resolution",
    "u_time",
};

static bool compute_size(Display_t *display, const Display_Configuration_t *configuration, GL_Point_t *position)
{
    int display_width, display_height;
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), NULL, NULL, &display_width, &display_height);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> display size is %dx%d", display_width, display_height);

    display->window_width = configuration->width; // TODO: width/height set to `0` means fit the display?
    display->window_height = configuration->height;
    display->window_scale = 1;

    int max_scale = imin(display_width / configuration->width, display_height / configuration->height);
    int scale = configuration->scale != 0 ? configuration->scale : max_scale;

    if (max_scale == 0) {
        Log_write(LOG_LEVELS_FATAL, "<DISPLAY> requested display size can't fit display!");
        return false;
    } else
    if (scale > max_scale) {
        Log_write(LOG_LEVELS_WARNING, "<DISPLAY> requested scaling x%d too big, forcing to x%d", scale, max_scale);
        scale = max_scale;
    }

    display->window_width = configuration->width * scale;
    display->window_height = configuration->height * scale;
    display->window_scale = scale;

    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> window size is %dx%d (%dx)", display->window_width, display->window_height,
        display->window_scale);

    int x = (display_width - display->window_width) / 2;
    int y = (display_height - display->window_height) / 2;
    if (!configuration->fullscreen) {
        display->vram_destination = (GL_Quad_t){
                0, 0, display->window_width, display->window_height
            };
        display->physical_width = display->window_width;
        display->physical_height = display->window_height;

        position->x = x;
        position->y = y;
    } else { // Toggle fullscreen by passing primary monitor!
        display->vram_destination = (GL_Quad_t){
                x, y, x + display->window_width, y + display->window_height
            };
        display->physical_width = display_width;
        display->physical_height = display_height;

        position->x = 0;
        position->y = 0;
    }

    return true;
}

static void error_callback(int error, const char *description)
{
    Log_write(LOG_LEVELS_ERROR, "<GLFW> %s", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
//    Log_write(LOG_LEVELS_TRACE, "<GLFW> key #%d is %d", scancode, action);
}

static void size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // Viewport matches window
    Log_write(LOG_LEVELS_DEBUG, "<GLFW> viewport size set to %dx%d", width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, 0.0, 1.0); // Configure top-left corner at <0, 0>
    glMatrixMode(GL_MODELVIEW); // Reset the model-view matrix.
    glLoadIdentity();
    Log_write(LOG_LEVELS_DEBUG, "<GLFW> projection/model matrix reset, going otho-2D");

    glEnable(GL_TEXTURE_2D); // Default, always enabled.
    glDisable(GL_DEPTH_TEST); // We just don't need it!
    glDisable(GL_STENCIL_TEST); // Ditto.
    glDisable(GL_BLEND); // Blending is disabled.
    glDisable(GL_ALPHA_TEST);
    Log_write(LOG_LEVELS_DEBUG, "<GLFW> optimizing OpenGL features");

#ifdef __DEBUG_TRIANGLES_WINDING__
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    Log_write(LOG_LEVELS_DEBUG, "<GLFW> enabling OpenGL debug");
#endif
}

bool Display_initialize(Display_t *display, const Display_Configuration_t *configuration, const char *title)
{
    *display = (Display_t){ 0 };

    display->configuration = *configuration;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        Log_write(LOG_LEVELS_FATAL, "<DISPLAY> can't initialize GLFW");
        return false;
    }

#if __GL_VERSION__ >= 0x0303
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
#endif
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
    glfwWindowHint(GLFW_FOCUSED, GLFW_TRUE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // We'll show it after the real-size has been calculated.

    display->window = glfwCreateWindow(1, 1, title, NULL, NULL); // 1x1 window, in order to have a context early.
    if (display->window == NULL) {
        Log_write(LOG_LEVELS_FATAL, "<DISPLAY> can't create window");
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(display->window);

    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> %sabling vertical synchronization", configuration->vertical_sync ? "en" : "dis");
    glfwSwapInterval(configuration->vertical_sync ? 1 : 0); // Set vertical sync, if required.

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        Log_write(LOG_LEVELS_FATAL, "<DISPLAY> can't initialize GLAD");
        glfwDestroyWindow(display->window);
        glfwTerminate();
        return false;
    }

    Log_write(LOG_LEVELS_INFO, "<DISPLAY> Vendor: %s", glGetString(GL_VENDOR));
    Log_write(LOG_LEVELS_INFO, "<DISPLAY> Renderer: %s", glGetString(GL_RENDERER));
    Log_write(LOG_LEVELS_INFO, "<DISPLAY> Version: %s", glGetString(GL_VERSION));
    Log_write(LOG_LEVELS_INFO, "<DISPLAY> GLSL: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glfwSetFramebufferSizeCallback(display->window, size_callback);
    glfwSetKeyCallback(display->window, key_callback);
    glfwSetInputMode(display->window, GLFW_CURSOR, configuration->hide_cursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

    if (!GL_context_create(&display->gl, configuration->width, configuration->height)) {
        Log_write(LOG_LEVELS_FATAL, "<DISPLAY> can't initialize GL");
        glfwDestroyWindow(display->window);
        glfwTerminate();
        return false;
    }

    GL_palette_greyscale(&display->palette, GL_MAX_PALETTE_COLORS);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> calculating greyscale palette of #%d entries", GL_MAX_PALETTE_COLORS);

    GL_Point_t position;
    if (!compute_size(display, configuration, &position)) {
        glfwDestroyWindow(display->window);
        glfwTerminate();
        return false;
    }

    if (!configuration->fullscreen) {
        glfwSetWindowMonitor(display->window, NULL, position.x, position.y, display->physical_width, display->physical_height, GLFW_DONT_CARE);
        glfwShowWindow(display->window);
    } else { // Toggle fullscreen by passing primary monitor!
        Log_write(LOG_LEVELS_INFO, "<DISPLAY> entering full-screen mode");
        glfwSetWindowMonitor(display->window, glfwGetPrimaryMonitor(), position.x, position.y, display->physical_width, display->physical_height, GLFW_DONT_CARE);
    }

    display->vram = malloc(display->configuration.width * display->configuration.width * sizeof(GL_Color_t));
    if (!display->vram) {
        Log_write(LOG_LEVELS_FATAL, "<DISPLAY> can't allocate VRAM buffer");
        GL_context_delete(&display->gl);
        glfwDestroyWindow(display->window);
        glfwTerminate();
    }
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> VRAM allocated at #%p (%dx%d)", display->vram, display->configuration.width, display->configuration.height);

    glGenTextures(1, &display->vram_texture); //allocate the memory for texture
    if (display->vram_texture == 0) {
        Log_write(LOG_LEVELS_FATAL, "<DISPLAY> can't allocate VRAM texture");
        free(display->vram);
        GL_context_delete(&display->gl);
        glfwDestroyWindow(display->window);
        glfwTerminate();
        return false;
    }
    glBindTexture(GL_TEXTURE_2D, display->vram_texture); // The VRAM texture is always the active and bound one.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0); // Disable mip-mapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, display->configuration.width, display->configuration.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> texture created w/ id #%d (%dx%d)", display->vram_texture, display->configuration.width, display->configuration.height);

    for (size_t i = 0; i < Display_Programs_t_CountOf; ++i) {
        const Program_Data_t *data = &_programs_data[i];
        if (!data->vertex_shader || !data->fragment_shader) {
            continue;
        }
        Program_t *program= &display->programs[i];
        if (!program_create(program) ||
            !program_attach(program, data->vertex_shader, PROGRAM_SHADER_VERTEX) ||
            !program_attach(program, data->fragment_shader, PROGRAM_SHADER_FRAGMENT)) {
            Log_write(LOG_LEVELS_FATAL, "<DISPLAY> can't initialize shaders");
            for (size_t j = 0; j < i; ++j) {
                program_delete(&display->programs[j]);
            }
            glDeleteBuffers(1, &display->vram_texture);
            free(display->vram);
            GL_context_delete(&display->gl);
            glfwDestroyWindow(display->window);
            glfwTerminate();
            return false;
        }

        program_prepare(program, _uniforms, Uniforms_t_CountOf);
        Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> program #%p prepared w/ id #%d", program, program->id);
    }

    Display_shader(display, NULL); // Use pass-thru at the beginning.

    return true;
}

void Display_terminate(Display_t *display)
{
    for (size_t i = 0; i < Display_Programs_t_CountOf; ++i) {
        if (display->programs[i].id == 0) {
            continue;
        }
        program_delete(&display->programs[i]);
    }

    glDeleteBuffers(1, &display->vram_texture);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> texture w/ id #%d deleted", display->vram_texture);

    free(display->vram);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> VRAM buffer #%p deallocated", display->vram);

    GL_context_delete(&display->gl);

    glfwDestroyWindow(display->window);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> windows #%p destroyed", display->window);

    glfwTerminate();
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> terminated");
}

bool Display_should_close(Display_t *display)
{
    return glfwWindowShouldClose(display->window);
}

void Display_update(Display_t *display, float delta_time)
{
    display->time += (GLfloat)delta_time;
    program_send(display->active_program, UNIFORM_TIME, PROGRAM_UNIFORM_FLOAT, 1, &display->time);
}

void Display_present(Display_t *display)
{
    GL_surface_to_rgba(&display->gl.buffer, &display->palette, display->vram);

#ifdef __GL_BGRA_PALETTE__
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, display->gl.buffer.width, display->gl.buffer.height, GL_BGRA, GL_UNSIGNED_BYTE, display->vram);
#else
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, display->gl.buffer.width, display->gl.buffer.height, GL_RGBA, GL_UNSIGNED_BYTE, display->vram);
#endif

    glBegin(GL_TRIANGLE_STRIP);
//        glColor4ub(255, 255, 255, 255); // Change this color to "tint".

        glTexCoord2f(0, 0); // CCW strip, top-left is <0,0> (the face direction of the strip is determined by the winding of the first triangle)
        glVertex2f(display->vram_destination.x0, display->vram_destination.y0);
        glTexCoord2f(0, 1);
        glVertex2f(display->vram_destination.x0, display->vram_destination.y1);
        glTexCoord2f(1, 0);
        glVertex2f(display->vram_destination.x1, display->vram_destination.y0);
        glTexCoord2f(1, 1);
        glVertex2f(display->vram_destination.x1, display->vram_destination.y1);
    glEnd();

    glfwSwapBuffers(display->window);
}

void Display_shader(Display_t *display, const char *effect)
{
    bool is_passthru = display->active_program == &display->programs[DISPLAY_PROGRAM_PASSTHRU];

    if (!is_passthru) {
        if (display->active_program) {
            program_delete(display->active_program);
        }
    } else
    if (!effect) {
        Log_write(LOG_LEVELS_INFO, "<DISPLAY> pass-thru shader already active, bailing out");
        return;
    }

    if (!effect) {
        Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> loading pass-thru shader");
        program_delete(&display->programs[DISPLAY_PROGRAM_CUSTOM]);
        display->active_program = &display->programs[DISPLAY_PROGRAM_PASSTHRU];
    } else {
        Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> loading custom shader");
        const size_t length = strlen(FRAGMENT_SHADER_CUSTOM) + strlen(effect);
        char *code = malloc((length + 1) * sizeof(char)); // Add null terminator for the string.
        strcpy(code, FRAGMENT_SHADER_CUSTOM);
        strcat(code, effect);

        Program_t *program = &display->programs[DISPLAY_PROGRAM_CUSTOM];

        if (program_create(program) &&
            program_attach(program, VERTEX_SHADER, PROGRAM_SHADER_VERTEX) &&
            program_attach(program, code, PROGRAM_SHADER_FRAGMENT)) {
            program_prepare(program, _uniforms, Uniforms_t_CountOf);
            display->active_program = program;
        } else {
            program_delete(program);
            Log_write(LOG_LEVELS_WARNING, "<DISPLAY> can't load custom shader");
        }

        free(code);
    }

    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> switched to program #%p", display->active_program);

    program_use(display->active_program);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> program #%p active", display->active_program);

    program_send(display->active_program, UNIFORM_TEXTURE, PROGRAM_UNIFORM_TEXTURE, 1, &_texture_id_0); // Redundant
    GLfloat resolution[] = { (GLfloat)display->window_width, (GLfloat)display->window_height };
    program_send(display->active_program, UNIFORM_RESOLUTION, PROGRAM_UNIFORM_VEC2, 1, resolution);
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> program #%p initialized", display->active_program);
}

void Display_palette(Display_t *display, const GL_Palette_t *palette)
{
    display->palette = *palette;
    Log_write(LOG_LEVELS_DEBUG, "<DISPLAY> palette updated");
}