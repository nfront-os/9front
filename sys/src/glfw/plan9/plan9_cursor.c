//========================================================================
// GLFW 3.5 for 9front - www.glfw.org
// SPDX-License-Identifier: Zlib
//------------------------------------------------------------------------
// The plan9 platform's cursors.  Plan 9's cursor is 16x16 pixels of two
// bits, clr (white) and set (black, over clr), and an offset from the
// pointer to its top left: written to /dev/cursor as mouse(3)'s 72 bytes
// (rio and wsys keep one a window and show it while the pointer is in
// it; a write of nothing gives the default arrow).
// - glfwCreateCursor: the image's pixels, opaque ones dark or light,
//   scaled down to 16x16 if larger
// - the standard cursors: drawn here (as SDL's plan9 driver draws them);
//   the arrow is the window system's
//========================================================================

#include "internal.h"

#if defined(_GLFW_PLAN9)

#include <string.h>

// the standard cursors: # black, o white, . nothing; black gets a white edge
static const struct
{
    int shape;
    int hotx, hoty;
    const char* rows[16];
} art[] =
{
    { GLFW_IBEAM_CURSOR, 7, 7, {
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
    { GLFW_CROSSHAIR_CURSOR, 7, 7, {
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
    { GLFW_RESIZE_NWSE_CURSOR, 7, 7, {
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
    { GLFW_RESIZE_NESW_CURSOR, 7, 7, {
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
    { GLFW_RESIZE_EW_CURSOR, 7, 7, {
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
    { GLFW_RESIZE_NS_CURSOR, 7, 7, {
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
    { GLFW_RESIZE_ALL_CURSOR, 7, 7, {
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
    { GLFW_NOT_ALLOWED_CURSOR, 7, 7, {
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
    { GLFW_POINTING_HAND_CURSOR, 4, 0, {
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

static void put32(unsigned char* p, int v)
{
    p[0] = (unsigned char) v;
    p[1] = (unsigned char) (v >> 8);
    p[2] = (unsigned char) (v >> 16);
    p[3] = (unsigned char) (v >> 24);
}

// pixel x y: 1 white, 2 black
static void setpix(_GLFWcursorPlan9* c, int x, int y, int v)
{
    const unsigned char bit = (unsigned char) (0x80 >> (x & 7));
    const int i = y * 2 + (x >> 3);

    if (v == 1)
        c->c[8 + i] |= bit;
    else if (v == 2)
        c->c[8 + 32 + i] |= bit;
}

GLFWbool _glfwCreateCursorPlan9(_GLFWcursor* cursor,
                                const GLFWimage* image,
                                int xhot, int yhot)
{
    _GLFWcursorPlan9* c = &cursor->plan9;
    const unsigned char* p;
    int x, y, w, h, scale;

    memset(c, 0, sizeof(*c));
    // larger than Plan 9's 16x16: every scale-th pixel
    scale = ((image->width > image->height ? image->width : image->height) + 15) / 16;
    w = image->width / scale;
    h = image->height / scale;
    if (w > 16)
        w = 16;
    if (h > 16)
        h = 16;
    for (y = 0;  y < h;  y++)
    {
        for (x = 0;  x < w;  x++)
        {
            p = image->pixels + ((size_t) y * scale * image->width + (size_t) x * scale) * 4;
            if (p[3] >= 128)
                setpix(c, x, y, p[0] * 299 + p[1] * 587 + p[2] * 114 < 128 * 1000 ? 2 : 1);
        }
    }
    xhot /= scale;
    yhot /= scale;
    put32(c->c, -(xhot < 0 ? 0 : xhot > 15 ? 15 : xhot));
    put32(c->c + 4, -(yhot < 0 ? 0 : yhot > 15 ? 15 : yhot));
    return GLFW_TRUE;
}

GLFWbool _glfwCreateStandardCursorPlan9(_GLFWcursor* cursor, int shape)
{
    _GLFWcursorPlan9* c = &cursor->plan9;
    char ch;
    int i, x, y, dx, dy, edge;

    memset(c, 0, sizeof(*c));
    for (i = 0;  i < (int) (sizeof(art) / sizeof(art[0]));  i++)
    {
        if (art[i].shape == shape)
            break;
    }
    if (i == (int) (sizeof(art) / sizeof(art[0])))
    {
        c->arrow = GLFW_TRUE;   // GLFW_ARROW_CURSOR, and what is not drawn here
        return GLFW_TRUE;
    }
    for (y = 0;  y < 16;  y++)
    {
        for (x = 0;  x < 16;  x++)
        {
            ch = art[i].rows[y][x];
            if (ch == '#')
            {
                setpix(c, x, y, 2);
                continue;
            }
            edge = ch == 'o';
            for (dy = -1;  dy <= 1 && !edge;  dy++)
            {
                for (dx = -1;  dx <= 1 && !edge;  dx++)
                {
                    if (x + dx >= 0 && x + dx < 16 && y + dy >= 0 && y + dy < 16 &&
                        art[i].rows[y + dy][x + dx] == '#')
                    {
                        edge = 1;
                    }
                }
            }
            if (edge)
                setpix(c, x, y, 1);
        }
    }
    put32(c->c, -art[i].hotx);
    put32(c->c + 4, -art[i].hoty);
    return GLFW_TRUE;
}

void _glfwDestroyCursorPlan9(_GLFWcursor* cursor)
{
    (void) cursor;
}

#endif // _GLFW_PLAN9
