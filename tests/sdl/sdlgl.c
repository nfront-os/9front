/*
 * SPDX-License-Identifier: MIT
 *
 * sdlgl: SDL3's OpenGL on 9front (sys/src/sdl3/plan9/video/SDL_plan9opengl.c:
 * EGL's plan9 platform, zink) in a window of wsys or rio: a context
 * (SDL_GL_CreateContext), a green square on dark blue (scissored clears)
 * for n frames, read back with glReadPixels and from the window
 * (/dev/window, image(6)); then SDL's GL renderer drawing the same.
 * Links -lvulkan (Mesa's EGL and GL are in it).
 * -W: not from the window (wsys shows the frames on an overlay plane).
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * usage: sdlgl [-W] [frames]
 */
#include <SDL3/SDL.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { W = 320, H = 240 };

/* GL's, by SDL_GL_GetProcAddress: no GL headers */
typedef unsigned int GLenum;
typedef unsigned char GLubyte;
#define GL_COLOR_BUFFER_BIT	0x4000
#define GL_SCISSOR_TEST		0x0C11
#define GL_RGBA			0x1908
#define GL_UNSIGNED_BYTE	0x1401
#define GL_RENDERER		0x1F01
#define GL_VERSION		0x1F02
static void (*glClearColor)(float, float, float, float);
static void (*glClear)(unsigned);
static void (*glScissor)(int, int, int, int);
static void (*glEnable)(GLenum);
static void (*glDisable)(GLenum);
static void (*glReadPixels)(int, int, int, int, GLenum, GLenum, void *);
static void (*glFinish)(void);
static const GLubyte *(*glGetString)(GLenum);

static int nfail;

#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); nfail++; } while(0)

static char *slurp(const char *path, long *len)
{
    char *b = NULL;
    long n = 0, m;
    int fd;

    if ((fd = open(path, O_RDONLY)) < 0) {
        return NULL;
    }
    for (;;) {
        b = realloc(b, n + 65536);
        if ((m = read(fd, b + n, 65536)) <= 0) {
            break;
        }
        n += m;
    }
    close(fd);
    *len = n;
    return b;
}

/* the window's pixel at x y inside its border of 4, from /dev/window (as sdltest) */
static int pixel(int x, int y, unsigned *rgb)
{
    char *b, f[13];
    long len, off;
    int minx, miny, maxx;
    unsigned char *p;

    if (!(b = slurp("/dev/window", &len)) || len < 60) {
        free(b);
        return -1;
    }
    memcpy(f, b, 12);
    f[12] = 0;
    if (!strstr(f, "x8r8g8b8") && !strstr(f, "a8r8g8b8")) {
        free(b);
        return -2;
    }
    minx = atoi(b + 12);
    miny = atoi(b + 24);
    maxx = atoi(b + 36);
    x += minx + 4;
    y += miny + 4;
    off = 60 + ((long)(y - miny) * (maxx - minx) + (x - minx)) * 4;
    if (off + 4 > len) {
        free(b);
        return -3;
    }
    p = (unsigned char *)b + off;
    *rgb = (unsigned)(p[0] | p[1] << 8 | p[2] << 16);
    free(b);
    return 0;
}

static void checkwin(const char *what, int x, int y, unsigned want)
{
    unsigned got;
    int r = pixel(x, y, &got);

    if (r < 0) {
        FAIL("%s: /dev/window at %d,%d: %d", what, x, y, r);
    } else if (got != want) {
        FAIL("%s: window at %d,%d %06x, want %06x", what, x, y, got, want);
    } else {
        OK("%s: window at %d,%d %06x", what, x, y, got);
    }
}

/* GL's pixel at x y from the top (GL counts from the bottom) */
static void checkgl(const char *what, int h, int x, int y, unsigned want)
{
    unsigned char p[4];
    unsigned got;

    glReadPixels(x, h - 1 - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, p);
    got = (unsigned)p[0] << 16 | (unsigned)p[1] << 8 | p[2];
    if (got != want) {
        FAIL("%s: glReadPixels at %d,%d %06x, want %06x", what, x, y, got, want);
    } else {
        OK("%s: glReadPixels at %d,%d %06x", what, x, y, got);
    }
}

static void square(int w, int h)
{
    glDisable(GL_SCISSOR_TEST);
    glClearColor(0, 0, 0x66 / 255.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_SCISSOR_TEST);
    glScissor(w / 4, h / 4, w / 2, h / 2);
    glClearColor(0, 1, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

#define GET(f) (*(void **)&f = (void *)SDL_GL_GetProcAddress(#f)) != NULL

int main(int argc, char **argv)
{
    SDL_Window *win;
    SDL_GLContext ctx;
    SDL_Renderer *ren;
    SDL_Surface *s;
    SDL_Event ev;
    SDL_FRect fr;
    Uint8 r, g, b;
    int w, h, n, f, nowin;
    Uint64 t0, t1;

    nowin = argc > 1 && strcmp(argv[1], "-W") == 0;
    if (nowin) {
        argc--;
        argv++;
    }
    n = argc > 1 ? atoi(argv[1]) : 60;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("FAIL SDL_Init: %s\nsdlgl: failed\n", SDL_GetError());
        return 1;
    }
    if (!(win = SDL_CreateWindow("sdlgl", W, H, SDL_WINDOW_OPENGL))) {
        printf("FAIL SDL_CreateWindow (OpenGL): %s\nsdlgl: failed\n", SDL_GetError());
        return 1;
    }
    OK("OpenGL window");
    if (!(ctx = SDL_GL_CreateContext(win))) {
        printf("FAIL SDL_GL_CreateContext: %s\nsdlgl: failed\n", SDL_GetError());
        return 1;
    }
    if (!(GET(glClearColor) && GET(glClear) && GET(glScissor) && GET(glEnable) && GET(glDisable) &&
          GET(glReadPixels) && GET(glFinish) && GET(glGetString))) {
        printf("FAIL SDL_GL_GetProcAddress: %s\nsdlgl: failed\n", SDL_GetError());
        return 1;
    }
    OK("context: %s, %s", (const char *)glGetString(GL_VERSION), (const char *)glGetString(GL_RENDERER));
    SDL_GetWindowSizeInPixels(win, &w, &h);

    t0 = SDL_GetTicksNS();
    for (f = 0; f < n; f++) {
        while (SDL_PollEvent(&ev)) {
        }
        square(w, h);
        if (f == n - 1) {
            glFinish();
            checkgl("middle", h, w / 2, h / 2, 0x00ff00);
            checkgl("corner", h, 4, 4, 0x000066);
        }
        if (!SDL_GL_SwapWindow(win)) {
            FAIL("SDL_GL_SwapWindow, frame %d: %s", f, SDL_GetError());
            break;
        }
    }
    t1 = SDL_GetTicksNS();
    if (f == n) {
        OK("%d frames in %.2f s", n, (t1 - t0) / 1e9);
    }
    if (!nowin) {
        SDL_Delay(100);
        checkwin("middle", w / 2, h / 2, 0x00ff00);
        checkwin("corner", 4, 4, 0x000066);
    }
    SDL_GL_DestroyContext(ctx);

    /* SDL's GL renderer over the same window */
    if (!(ren = SDL_CreateRenderer(win, "opengl"))) {
        FAIL("SDL_CreateRenderer opengl: %s", SDL_GetError());
    } else {
        OK("renderer %s", SDL_GetRendererName(ren));
        for (f = 0; f < 10; f++) {
            SDL_SetRenderDrawColor(ren, 0, 0, 0x66, 255);
            SDL_RenderClear(ren);
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            fr = (SDL_FRect){ (float)(W / 4), (float)(H / 4), (float)(W / 2), (float)(H / 2) };
            SDL_RenderFillRect(ren, &fr);
            if (f == 9) {
                if (!(s = SDL_RenderReadPixels(ren, NULL)) || !SDL_ReadSurfacePixel(s, W / 2, H / 2, &r, &g, &b, NULL)) {
                    FAIL("renderer: read back: %s", SDL_GetError());
                } else if (r != 0 || g != 255 || b != 0) {
                    FAIL("renderer: middle %02x%02x%02x, want 00ff00", r, g, b);
                } else {
                    OK("renderer: middle 00ff00");
                }
                SDL_DestroySurface(s);
            }
            SDL_RenderPresent(ren);
        }
        SDL_DestroyRenderer(ren);
    }
    SDL_DestroyWindow(win);
    SDL_Quit();
    printf(nfail ? "sdlgl: %d failed\n" : "sdlgl: all passed\n", nfail);
    return nfail != 0;
}
