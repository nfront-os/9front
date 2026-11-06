/*
  SPDX-License-Identifier: Zlib

  SDL's plan9 video driver: cursors.  Plan 9's cursor is 16x16 pixels of
  two bits, clr (white) and set (black, over clr), and an offset from the
  pointer to its top left: written to /dev/cursor as mouse(3)'s 72 bytes
  (rio and wsys keep one a window and show it while the pointer is in it;
  a write of nothing gives the default arrow).
  - SDL_CreateCursor/SDL_CreateColorCursor: the surface's pixels, opaque
    ones dark or light, scaled down to 16x16 if larger
  - system cursors: drawn here; the default is the window system's arrow
  - hidden: a cursor of no pixels
*/
#include "SDL_internal.h"

#ifdef SDL_VIDEO_DRIVER_PLAN9

#include "video/SDL_sysvideo.h"
#include "events/SDL_mouse_c.h"
#include "SDL_plan9mouse.h"

#include <fcntl.h>
#include <unistd.h>

enum { Ncursor = 2 * 4 + 2 * 2 * 16 };

struct SDL_CursorData {
    Uint8 c[Ncursor];   /* offset x y (little endian), clr, set */
};

static int cursorfd = -1;

/* the system cursors: # black, o white, . nothing; black gets a white edge */
static const struct {
    SDL_SystemCursor id;
    int hotx, hoty;
    const char *rows[16];
} art[] = {
    { SDL_SYSTEM_CURSOR_TEXT, 7, 7, {
        "....###.###.....",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        "....###.###.....",
        "................",
        "................" } },
    { SDL_SYSTEM_CURSOR_WAIT, 7, 7, {
        "..###########...",
        "..#.........#...",
        "...#.......#....",
        "....#.....#.....",
        ".....#...#......",
        "......#.#.......",
        ".......#........",
        "......#.#.......",
        ".....#...#......",
        "....#..#..#.....",
        "...#..###..#....",
        "..#..#####..#...",
        "..###########...",
        "................",
        "................",
        "................" } },
    { SDL_SYSTEM_CURSOR_CROSSHAIR, 7, 7, {
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        "................",
        "................",
        "#####.....#####.",
        "................",
        "................",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        "................" } },
    { SDL_SYSTEM_CURSOR_NWSE_RESIZE, 7, 7, {
        "................",
        ".#####..........",
        ".##.............",
        ".#.#............",
        ".#..#...........",
        ".#...#..........",
        "......#.........",
        ".......#........",
        "........#.......",
        ".........#...#..",
        "..........#..#..",
        "...........#.#..",
        "............##..",
        ".........#####..",
        "................",
        "................" } },
    { SDL_SYSTEM_CURSOR_NESW_RESIZE, 7, 7, {
        "................",
        ".........#####..",
        "............##..",
        "...........#.#..",
        "..........#..#..",
        ".........#...#..",
        "........#.......",
        ".......#........",
        "......#.........",
        ".#...#..........",
        ".#..#...........",
        ".#.#............",
        ".##.............",
        ".#####..........",
        "................",
        "................" } },
    { SDL_SYSTEM_CURSOR_EW_RESIZE, 7, 7, {
        "................",
        "................",
        "................",
        "................",
        "................",
        "..#.........#...",
        ".##.........##..",
        "###############.",
        ".##.........##..",
        "..#.........#...",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................" } },
    { SDL_SYSTEM_CURSOR_NS_RESIZE, 7, 7, {
        ".......#........",
        "......###.......",
        ".....#####......",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".......#........",
        ".....#####......",
        "......###.......",
        ".......#........",
        "................" } },
    { SDL_SYSTEM_CURSOR_MOVE, 7, 7, {
        ".......#........",
        "......###.......",
        ".....#.#.#......",
        ".......#........",
        ".......#........",
        "..#....#....#...",
        ".##....#....##..",
        "###############.",
        ".##....#....##..",
        "..#....#....#...",
        ".......#........",
        ".......#........",
        ".....#.#.#......",
        "......###.......",
        ".......#........",
        "................" } },
    { SDL_SYSTEM_CURSOR_NOT_ALLOWED, 7, 7, {
        "................",
        "......###.......",
        "....#######.....",
        "...##.....##....",
        "..###......##...",
        "..#..#......#...",
        ".##...#.....##..",
        ".##....#....##..",
        ".##.....#...##..",
        "..#......#..#...",
        "..##......###...",
        "...##.....##....",
        "....#######.....",
        "......###.......",
        "................",
        "................" } },
    { SDL_SYSTEM_CURSOR_POINTER, 4, 0, {
        "....##..........",
        "...#oo#.........",
        "...#oo#.........",
        "...#oo#.........",
        "...#oo###.......",
        "...#oo#oo###....",
        ".###oo#oo#oo##..",
        "#oo#oooooooo#o#.",
        "#oooooooooooooo#",
        ".#ooooooooooooo#",
        "..#oooooooooooo#",
        "..#ooooooooooo#.",
        "...#oooooooooo#.",
        "....#oooooooo#..",
        ".....########...",
        "................" } },
};

/* the ones SDL lets fall back to another */
static SDL_SystemCursor same(SDL_SystemCursor id)
{
    switch (id) {
    case SDL_SYSTEM_CURSOR_PROGRESS:
        return SDL_SYSTEM_CURSOR_WAIT;
    case SDL_SYSTEM_CURSOR_NW_RESIZE:
    case SDL_SYSTEM_CURSOR_SE_RESIZE:
        return SDL_SYSTEM_CURSOR_NWSE_RESIZE;
    case SDL_SYSTEM_CURSOR_NE_RESIZE:
    case SDL_SYSTEM_CURSOR_SW_RESIZE:
        return SDL_SYSTEM_CURSOR_NESW_RESIZE;
    case SDL_SYSTEM_CURSOR_E_RESIZE:
    case SDL_SYSTEM_CURSOR_W_RESIZE:
        return SDL_SYSTEM_CURSOR_EW_RESIZE;
    case SDL_SYSTEM_CURSOR_N_RESIZE:
    case SDL_SYSTEM_CURSOR_S_RESIZE:
        return SDL_SYSTEM_CURSOR_NS_RESIZE;
    default:
        return id;
    }
}

static void put32(Uint8 *p, int v)
{
    p[0] = (Uint8)v;
    p[1] = (Uint8)(v >> 8);
    p[2] = (Uint8)(v >> 16);
    p[3] = (Uint8)(v >> 24);
}

/* pixel x y: 0 nothing, 1 white, 2 black */
static void setpix(SDL_CursorData *d, int x, int y, int v)
{
    Uint8 bit = (Uint8)(0x80 >> (x & 7));
    int i = y * 2 + (x >> 3);

    if (v == 1) {
        d->c[8 + i] |= bit;
    } else if (v == 2) {
        d->c[8 + 32 + i] |= bit;
    }
}

static SDL_Cursor *newcursor(SDL_CursorData *d)
{
    SDL_Cursor *c = (SDL_Cursor *)SDL_calloc(1, sizeof(*c));

    if (!c) {
        SDL_free(d);
        return NULL;
    }
    c->internal = d;
    return c;
}

static SDL_Cursor *PLAN9_CreateCursor(SDL_Surface *surface, int hot_x, int hot_y)
{
    SDL_Surface *s;
    SDL_CursorData *d;
    Uint8 r, g, b, a;
    int x, y, scale, w, h;

    if (!(s = SDL_ConvertSurface(surface, SDL_PIXELFORMAT_ARGB8888))) {
        return NULL;
    }
    if (!(d = (SDL_CursorData *)SDL_calloc(1, sizeof(*d)))) {
        SDL_DestroySurface(s);
        return NULL;
    }
    /* larger than Plan 9's 16x16: every scale-th pixel */
    scale = (SDL_max(s->w, s->h) + 15) / 16;
    w = SDL_min(16, s->w / scale);
    h = SDL_min(16, s->h / scale);
    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            SDL_ReadSurfacePixel(s, x * scale, y * scale, &r, &g, &b, &a);
            if (a >= 128) {
                setpix(d, x, y, r * 299 + g * 587 + b * 114 < 128 * 1000 ? 2 : 1);
            }
        }
    }
    SDL_DestroySurface(s);
    put32(d->c, -SDL_clamp(hot_x / scale, 0, 15));
    put32(d->c + 4, -SDL_clamp(hot_y / scale, 0, 15));
    return newcursor(d);
}

static SDL_Cursor *PLAN9_CreateSystemCursor(SDL_SystemCursor id)
{
    SDL_CursorData *d;
    char c;
    int i, x, y, dx, dy, edge;

    id = same(id);
    if (id == SDL_SYSTEM_CURSOR_DEFAULT) {
        return newcursor(NULL);
    }
    for (i = 0; i < (int)SDL_arraysize(art); i++) {
        if (art[i].id == id) {
            break;
        }
    }
    if (i == (int)SDL_arraysize(art)) {
        return newcursor(NULL);
    }
    if (!(d = (SDL_CursorData *)SDL_calloc(1, sizeof(*d)))) {
        return NULL;
    }
    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            c = art[i].rows[y][x];
            if (c == '#') {
                setpix(d, x, y, 2);
                continue;
            }
            edge = c == 'o';
            for (dy = -1; dy <= 1 && !edge; dy++) {
                for (dx = -1; dx <= 1 && !edge; dx++) {
                    if (x + dx >= 0 && x + dx < 16 && y + dy >= 0 && y + dy < 16 && art[i].rows[y + dy][x + dx] == '#') {
                        edge = 1;
                    }
                }
            }
            if (edge) {
                setpix(d, x, y, 1);
            }
        }
    }
    put32(d->c, -art[i].hotx);
    put32(d->c + 4, -art[i].hoty);
    return newcursor(d);
}

static bool PLAN9_ShowCursor(SDL_Cursor *cursor)
{
    static const SDL_CursorData none;
    const SDL_CursorData *d;

    if (cursorfd < 0 && (cursorfd = open("/dev/cursor", O_WRONLY)) < 0) {
        return SDL_SetError("plan9: no /dev/cursor");
    }
    d = cursor ? cursor->internal : &none;
    if (!d) {
        return write(cursorfd, "", 0) == 0 ? true : SDL_SetError("plan9: /dev/cursor: write failed");
    }
    return write(cursorfd, d->c, Ncursor) == Ncursor ? true : SDL_SetError("plan9: /dev/cursor: write failed");
}

static void PLAN9_FreeCursor(SDL_Cursor *cursor)
{
    if (cursor) {
        SDL_free(cursor->internal);
        SDL_free(cursor);
    }
}

void PLAN9_InitMouse(void)
{
    SDL_Mouse *mouse = SDL_GetMouse();

    mouse->CreateCursor = PLAN9_CreateCursor;
    mouse->CreateSystemCursor = PLAN9_CreateSystemCursor;
    mouse->ShowCursor = PLAN9_ShowCursor;
    mouse->FreeCursor = PLAN9_FreeCursor;
    SDL_SetDefaultCursor(PLAN9_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
}

void PLAN9_QuitMouse(void)
{
    if (cursorfd >= 0) {
        write(cursorfd, "", 0);   /* the window system's arrow again */
        close(cursorfd);
        cursorfd = -1;
    }
}

#endif /* SDL_VIDEO_DRIVER_PLAN9 */
