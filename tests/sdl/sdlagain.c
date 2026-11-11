/*
 * SPDX-License-Identifier: MIT
 *
 * sdlagain: an SDL3 window that outlives its wsys (tests/sdl/sdlagain.rc):
 * the window surface filled red and shown every 50 ms for secs seconds;
 * the window events it gets (resized, exposed, quit) a line each; at the end
 * how many updates failed since the last exposed (none: it drew on in the
 * window of the next wsys); a note that comes, as "signal n" (SIGINT, SIGTERM,
 * SIGHUP caught here: SDL's would make them a quit). Prints "ok ..." / "FAIL ..." lines and
 * "sdlagain: all passed".
 */
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

/* which note came, if one did (SDL makes SIGINT and SIGTERM a quit) */
static volatile sig_atomic_t gotsig;

static void
onsig(int s)
{
    gotsig = s;
}

int
main(int argc, char **argv)
{
    SDL_Window *win;
    SDL_Surface *s;
    SDL_Event e;
    Uint64 t0;
    int secs, frames = 0, failed = 0, exposed = 0, quit = 0;

    secs = argc > 1 ? atoi(argv[1]) : 20;
    setvbuf(stdout, NULL, _IOLBF, 0);
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("FAIL SDL_Init: %s\n", SDL_GetError());
        return 1;
    }
    if ((win = SDL_CreateWindow("sdlagain", 300, 200, 0)) == NULL) {
        printf("FAIL SDL_CreateWindow: %s\n", SDL_GetError());
        return 1;
    }
    printf("ok window\n");
    signal(SIGINT, onsig);
    signal(SIGTERM, onsig);
    signal(SIGHUP, onsig);
    t0 = SDL_GetTicks();
    while (!quit && SDL_GetTicks() - t0 < (Uint64)secs * 1000) {
        if (gotsig) {
            printf("signal %d (%s)\n", (int)gotsig, gotsig == SIGINT ? "interrupt" : gotsig == SIGTERM ? "term" : gotsig == SIGHUP ? "hangup" : "?");
            gotsig = 0;
        }
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_EVENT_WINDOW_RESIZED:
                printf("event resized %d %d\n", e.window.data1, e.window.data2);
                break;
            case SDL_EVENT_WINDOW_EXPOSED:
                printf("event exposed\n");
                exposed++;
                failed = 0;
                break;
            case SDL_EVENT_QUIT:
                printf("event quit\n");
                quit = 1;
                break;
            }
        }
        if ((s = SDL_GetWindowSurface(win)) != NULL) {
            SDL_FillSurfaceRect(s, NULL, SDL_MapSurfaceRGB(s, 255, 0, 0));
            if (!SDL_UpdateWindowSurface(win))
                failed++;
            else
                frames++;
        }
        SDL_Delay(50);
    }
    printf("ok %d frames, %d exposed, %d failed since\n", frames, exposed, failed);
    if (quit)
        printf("FAIL it ended (quit)\n");
    else if (failed > 0)
        printf("FAIL %d updates failed after the last exposed\n", failed);
    else
        printf("sdlagain: all passed\n");
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
