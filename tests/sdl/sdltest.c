/*
 * SPDX-License-Identifier: MIT
 *
 * sdltest: SDL3 on 9front (sys/src/sdl3, the plan9 drivers) in a window of
 * wsys or rio: the video driver's name, the window's size and title
 * (/dev/label), the window surface (left half red, right half blue) and
 * the software renderer (a green square) read back from the window
 * (/dev/window, image(6)), the clipboard (/dev/snarf), events pumped for
 * a while, a tone through /dev/audio if there is one.
 * -e secs: then the input events for secs seconds (keys, the mouse), a line
 * each (the host types and moves the mouse through QEMU: tests/sdl/README).
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <SDL3/SDL.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { W = 320, H = 240 };

static int nfail;

#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); nfail++; } while(0)

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

/*
 * the window's pixel at x y of its contents (inside the border of 4),
 * from /dev/window: image(6), five 12-byte fields (chan, the rectangle),
 * then x8r8g8b8 or r8g8b8a8-like 32-bit pixels
 */
static int pixel(int x, int y, unsigned *rgb)
{
    char *b, f[13];
    long len, off;
    int minx, miny, maxx, i;
    unsigned char *p;

    if (!(b = slurp("/dev/window", &len)) || len < 60) {
        free(b);
        return -1;
    }
    memcpy(f, b, 12);
    f[12] = 0;
    /* the fields are right-justified in 11 characters and a blank */
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
    i = p[0] | p[1] << 8 | p[2] << 16;
    *rgb = (unsigned)i;
    free(b);
    return 0;
}

static void check(const char *what, int x, int y, unsigned want)
{
    unsigned got;
    int r = pixel(x, y, &got);

    if (r < 0) {
        FAIL("%s: /dev/window at %d,%d: %d", what, x, y, r);
    } else if (got != want) {
        FAIL("%s: at %d,%d %06x, want %06x", what, x, y, got, want);
    } else {
        OK("%s: at %d,%d %06x", what, x, y, got);
    }
}

int main(int argc, char **argv)
{
    SDL_Window *win;
    SDL_Surface *s;
    SDL_Renderer *ren;
    SDL_Event ev;
    SDL_FRect fr;
    SDL_Rect r;
    char *t;
    long len;
    int w, h, n, i;
    Uint64 t0;

    int evsecs = argc > 2 && strcmp(argv[1], "-e") == 0 ? atoi(argv[2]) : 0;

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        printf("FAIL SDL_Init: %s\nsdltest: failed\n", SDL_GetError());
        return 1;
    }
    if (strcmp(SDL_GetCurrentVideoDriver(), "plan9") == 0) {
        OK("video driver %s", SDL_GetCurrentVideoDriver());
    } else {
        FAIL("video driver %s, not plan9", SDL_GetCurrentVideoDriver());
    }
    if (!(win = SDL_CreateWindow("sdltest", W, H, 0))) {
        printf("FAIL SDL_CreateWindow: %s\nsdltest: failed\n", SDL_GetError());
        return 1;
    }
    SDL_GetWindowSize(win, &w, &h);
    if (w == W && h == H) {
        OK("window %dx%d", w, h);
    } else {
        FAIL("window %dx%d, want %dx%d", w, h, W, H);
    }
    if ((t = slurp("/dev/label", &len)) && len == 7 && memcmp(t, "sdltest", 7) == 0) {
        OK("title in /dev/label");
    } else {
        FAIL("/dev/label: %.*s", t ? (int)len : 0, t ? t : "");
    }
    free(t);

    /* the window surface: halves */
    s = SDL_GetWindowSurface(win);
    if (!s) {
        FAIL("SDL_GetWindowSurface: %s", SDL_GetError());
    } else {
        r = (SDL_Rect){ 0, 0, w / 2, h };
        SDL_FillSurfaceRect(s, &r, SDL_MapSurfaceRGB(s, 255, 0, 0));
        r = (SDL_Rect){ w / 2, 0, w - w / 2, h };
        SDL_FillSurfaceRect(s, &r, SDL_MapSurfaceRGB(s, 0, 0, 255));
        if (!SDL_UpdateWindowSurface(win)) {
            FAIL("SDL_UpdateWindowSurface: %s", SDL_GetError());
        }
        check("surface left", w / 4, h / 2, 0xff0000);
        check("surface right", 3 * w / 4, h / 2, 0x0000ff);
        SDL_DestroyWindowSurface(win);
    }

    /* the software renderer */
    if (!(ren = SDL_CreateRenderer(win, NULL))) {
        FAIL("SDL_CreateRenderer: %s", SDL_GetError());
    } else {
        OK("renderer %s", SDL_GetRendererName(ren));
        SDL_SetRenderDrawColor(ren, 0, 0, 0x66, 255);
        SDL_RenderClear(ren);
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
        fr = (SDL_FRect){ (float)(w / 4), (float)(h / 4), (float)(w / 2), (float)(h / 2) };
        SDL_RenderFillRect(ren, &fr);
        SDL_RenderPresent(ren);
        check("renderer middle", w / 2, h / 2, 0x00ff00);
        check("renderer corner", 4, 4, 0x000066);
    }

    /* the clipboard */
    if (SDL_SetClipboardText("sdltest snarf") && (t = SDL_GetClipboardText()) && strcmp(t, "sdltest snarf") == 0) {
        OK("clipboard through /dev/snarf");
    } else {
        FAIL("clipboard: %s", SDL_GetError());
    }

    /* events for a while: whatever comes, no hang */
    t0 = SDL_GetTicks();
    n = 0;
    while (SDL_GetTicks() - t0 < 500) {
        while (SDL_PollEvent(&ev)) {
            n++;
        }
        SDL_Delay(10);
    }
    OK("events pumped for 0.5 s: %d", n);

    /* a tone, if there is a sound card */
    if (access("/dev/audio", F_OK) == 0 && SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        SDL_AudioSpec spec = { SDL_AUDIO_S16, 1, 22050 };
        SDL_AudioStream *as = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
        static Sint16 tone[4410];

        for (i = 0; i < (int)SDL_arraysize(tone); i++) {
            tone[i] = (Sint16)(i % 50 < 25 ? 3000 : -3000);
        }
        if (!as || !SDL_PutAudioStreamData(as, tone, sizeof tone) || !SDL_ResumeAudioStreamDevice(as)) {
            FAIL("audio: %s", SDL_GetError());
        } else {
            SDL_Delay(300);
            OK("audio: 0.2 s through %s", SDL_GetCurrentAudioDriver());
        }
        SDL_DestroyAudioStream(as);
    } else {
        OK("audio: no /dev/audio here");
    }

    /* -e: the input, as SDL gives it */
    if (evsecs > 0) {
        SDL_StartTextInput(win);
        printf("ready for events\n");
        fflush(stdout);
    }
    t0 = SDL_GetTicks();
    while (evsecs > 0 && SDL_GetTicks() - t0 < (Uint64)evsecs * 1000) {
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                printf("event key %s %s\n", ev.type == SDL_EVENT_KEY_DOWN ? "down" : "up", SDL_GetScancodeName(ev.key.scancode));
                break;
            case SDL_EVENT_TEXT_INPUT:
                printf("event text %s\n", ev.text.text);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                printf("event button %d %s at %.0f,%.0f\n", ev.button.button,
                       ev.type == SDL_EVENT_MOUSE_BUTTON_DOWN ? "down" : "up", ev.button.x, ev.button.y);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                printf("event motion %.0f,%.0f\n", ev.motion.x, ev.motion.y);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                printf("event wheel %.0f\n", ev.wheel.y);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                printf("event resized %dx%d\n", ev.window.data1, ev.window.data2);
                break;
            }
            fflush(stdout);
        }
        SDL_Delay(10);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    printf(nfail ? "sdltest: %d failed\n" : "sdltest: all passed\n", nfail);
    return nfail != 0;
}
