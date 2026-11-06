/*
 * SPDX-License-Identifier: MIT
 *
 * sdlvk: SDL3's Vulkan on 9front (sys/src/sdl3/plan9/video/SDL_plan9vulkan.c)
 * in a window of wsys or rio: the instance extensions SDL asks for
 * (VK_KHR_surface, VK_PLAN9_surface), SDL's Vulkan renderer (its surface
 * from SDL_Vulkan_CreateSurface, a swapchain of the plan9 WSI) drawing a
 * green square on dark blue for n frames, read back by the renderer and
 * from the window (/dev/window, image(6)).  Links -lvulkan.
 * -W: not from the window (wsys shows the frames on an overlay plane:
 * the window's image does not have them).
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * usage: sdlvk [-W] [frames]
 */
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { W = 320, H = 240 };

static int nfail;

#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); nfail++; } while(0)

/* the whole of a file */
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

static void checkread(const char *what, SDL_Surface *s, int x, int y, unsigned want)
{
    Uint8 r, g, b;
    unsigned got;

    if (!SDL_ReadSurfacePixel(s, x, y, &r, &g, &b, NULL)) {
        FAIL("%s: SDL_ReadSurfacePixel: %s", what, SDL_GetError());
        return;
    }
    got = (unsigned)r << 16 | (unsigned)g << 8 | b;
    if (got != want) {
        FAIL("%s: read back at %d,%d %06x, want %06x", what, x, y, got, want);
    } else {
        OK("%s: read back at %d,%d %06x", what, x, y, got);
    }
}

int main(int argc, char **argv)
{
    SDL_Window *win;
    SDL_Renderer *ren;
    SDL_Surface *s;
    SDL_Event ev;
    SDL_FRect fr;
    char const *const *ext;
    Uint32 next, i;
    int w, h, n, f, plan9, khr, nowin;
    Uint64 t0, t1, tev, tdraw, tpresent, tread = 0;

    nowin = argc > 1 && strcmp(argv[1], "-W") == 0;
    if (nowin) {
        argc--;
        argv++;
    }
    n = argc > 1 ? atoi(argv[1]) : 60;
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("FAIL SDL_Init: %s\nsdlvk: failed\n", SDL_GetError());
        return 1;
    }
    if (!(win = SDL_CreateWindow("sdlvk", W, H, SDL_WINDOW_VULKAN))) {
        printf("FAIL SDL_CreateWindow (Vulkan): %s\nsdlvk: failed\n", SDL_GetError());
        return 1;
    }
    OK("Vulkan window");
    plan9 = khr = 0;
    if (!(ext = SDL_Vulkan_GetInstanceExtensions(&next))) {
        FAIL("SDL_Vulkan_GetInstanceExtensions: %s", SDL_GetError());
    } else {
        for (i = 0; i < next; i++) {
            plan9 |= strcmp(ext[i], "VK_PLAN9_surface") == 0;
            khr |= strcmp(ext[i], "VK_KHR_surface") == 0;
        }
        if (plan9 && khr) {
            OK("instance extensions: VK_KHR_surface, VK_PLAN9_surface");
        } else {
            FAIL("instance extensions: %u, KHR_surface %d PLAN9_surface %d", next, khr, plan9);
        }
    }
    if (!(ren = SDL_CreateRenderer(win, "vulkan"))) {
        char path[64], *seg;
        long len;

        printf("FAIL SDL_CreateRenderer vulkan: %s\n", SDL_GetError());
        /* the address space, for a map that found no room */
        snprintf(path, sizeof path, "/proc/%d/segment", getpid());
        if ((seg = slurp(path, &len))) {
            printf("%.*s", (int)len, seg);
        }
        printf("sdlvk: failed\n");
        fflush(stdout);
        if (getenv("SDLVK_HOLD")) {
            SDL_Delay(atoi(getenv("SDLVK_HOLD")) * 1000);	/* to look at it from outside */
        }
        return 1;
    }
    OK("renderer %s", SDL_GetRendererName(ren));
    SDL_GetWindowSize(win, &w, &h);

    t0 = SDL_GetTicksNS();
    tev = tdraw = tpresent = 0;
    for (f = 0; f < n; f++) {
        Uint64 a = SDL_GetTicksNS(), b, c;

        while (SDL_PollEvent(&ev)) {
        }
        b = SDL_GetTicksNS();
        tev += b - a;
        SDL_SetRenderDrawColor(ren, 0, 0, 0x66, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
        fr = (SDL_FRect){ (float)(w / 4), (float)(h / 4), (float)(w / 2), (float)(h / 2) };
        SDL_RenderFillRect(ren, &fr);
        if (f == n - 1) {
            /* before the present: what the renderer drew */
            Uint64 r0 = SDL_GetTicksNS();

            s = SDL_RenderReadPixels(ren, NULL);
            tread = SDL_GetTicksNS() - r0;
            tdraw -= tread;
            if (!s) {
                FAIL("SDL_RenderReadPixels: %s", SDL_GetError());
            } else {
                checkread("middle", s, w / 2, h / 2, 0x00ff00);
                checkread("corner", s, 4, 4, 0x000066);
                SDL_DestroySurface(s);
            }
        }
        c = SDL_GetTicksNS();
        tdraw += c - b;
        if (!SDL_RenderPresent(ren)) {
            FAIL("SDL_RenderPresent, frame %d: %s", f, SDL_GetError());
            break;
        }
        tpresent += SDL_GetTicksNS() - c;
    }
    t1 = SDL_GetTicksNS();
    if (f == n) {
        OK("%d frames in %.2f s: each events %.2f ms, drawing %.2f ms, present %.2f ms; read back in %.2f ms", n,
           (t1 - t0) / 1e9, tev / 1e6 / n, tdraw / 1e6 / n, tpresent / 1e6 / n, tread / 1e6);
    }
    if (!nowin) {
        SDL_Delay(100);
        checkwin("middle", w / 2, h / 2, 0x00ff00);
        checkwin("corner", 4, 4, 0x000066);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    printf(nfail ? "sdlvk: %d failed\n" : "sdlvk: all passed\n", nfail);
    return nfail != 0;
}
