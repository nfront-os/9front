/*
  SPDX-License-Identifier: Zlib

  SDL's video driver for 9front ("plan9"): the process's window of rio or
  wsys (or the screen without either), as draw(3) gives it.

  - pixels: SDL's window framebuffer, loaded into the window with libdraw
    (p9draw.c, the native side) on each update
  - the mouse: /dev/mouse (m x y buttons msec; r when the window changed:
    it is looked up again, SDL hears of its new size)
  - the keyboard: /dev/kbd (kbd(3) as kbdfs(8) gives it: k and K list the
    keys down after a press or release, so each change is a key event;
    c the character typed, SDL's text input)
  - the title: /dev/label; the clipboard: /dev/snarf; the size: /dev/wctl
  - cursors: /dev/cursor (SDL_plan9mouse.c); warping the pointer: writing
    m x y to /dev/mouse
  - Vulkan: VK_PLAN9_surface (SDL_plan9vulkan.c)

  One window a process (its window of the window system); readers of
  /dev/mouse and /dev/kbd are threads of their own, PumpEvents takes what
  they read. When the window's wsys is gone the window is made again in the
  next one (libdraw's wsysagain) and drawn again: the program goes on.
*/
#include "SDL_internal.h"

#ifdef SDL_VIDEO_DRIVER_PLAN9

#include "video/SDL_sysvideo.h"
#include "events/SDL_events_c.h"
#include "events/SDL_keyboard_c.h"
#include "events/SDL_mouse_c.h"
#include "events/SDL_windowevents_c.h"
#include "SDL_plan9opengl.h"
#include "SDL_plan9mouse.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "p9draw.h"
#include "SDL_plan9vulkan.h"

#define PLAN9_DRIVER_NAME "plan9"

/* Plan 9's keyboard.h, its runes for keys that are not characters */
enum {
    KF = 0xF000, Spec = 0xF800,
    Khome = KF | 0x0D, Kup = KF | 0x0E, Kdown = Spec | 0x00, Kpgup = KF | 0x0F, Kprint = KF | 0x10,
    Kleft = KF | 0x11, Kright = KF | 0x12, Kpgdown = KF | 0x13, Kins = KF | 0x14, Kalt = KF | 0x15,
    Kshift = KF | 0x16, Kctl = KF | 0x17, Kend = KF | 0x18, Kscroll = KF | 0x19,
    Kbs = 0x08, Kdel = 0x7f, Kesc = 0x1b,
    Kbreak = Spec | 0x61, Kcaps = Spec | 0x64, Knum = Spec | 0x65, Kaltgr = Spec | 0x67, Kmod4 = Spec | 0x68,
    Borderwidth = 4,        /* rio's and wsys' */
};

/* what the readers read, for PumpEvents */
typedef struct Raw {
    char kind;       /* m mouse, r resize, k K c keyboard, A a new wsys, q the end */
    int x, y, b;
    char text[64];   /* k K c: UTF-8 */
} Raw;

enum { Nraw = 512, Nkeys = 32 };

static struct {
    SDL_Window *window;
    SDL_Mutex *lock;
    Raw raw[Nraw];
    int head, tail;
    int mousefd, kbdfd;
    bool readers;                /* started (once a process) */
    int wx, wy, ww, wh;          /* the window's contents on the screen */
    int buttons;
    Uint32 keys[Nkeys];          /* down, as the last k/K said */
    int nkeys;
    void *fb;
} p9 = { .mousefd = -1, .kbdfd = -1 };

static void push(const Raw *r)
{
    SDL_LockMutex(p9.lock);
    if ((p9.head + 1) % Nraw != p9.tail) {
        p9.raw[p9.head] = *r;
        p9.head = (p9.head + 1) % Nraw;
    }
    SDL_UnlockMutex(p9.lock);
}

static int SDLCALL kbdreader(void *arg);

/* SDL_PLAN9_DEBUG=1: what the readers do when the window's files fail */
static int debug(void)
{
    static int d = -1;

    if (d < 0) {
        const char *e = SDL_getenv("SDL_PLAN9_DEBUG");
        d = e != NULL && SDL_atoi(e) != 0;
    }
    return d;
}

/* the keyboard's reader, of the window's /dev/kbd now (its own descriptor) */
static void startkbd(void)
{
    if (p9.kbdfd >= 0) {
        SDL_DetachThread(SDL_CreateThread(kbdreader, "plan9 kbd", (void *)(intptr_t)p9.kbdfd));
    }
}

/*
 * m x y buttons msec, 49 bytes a message; r: the window changed. When the
 * window's files fail and its wsys is gone, the window in the next one
 * (p9draw_again): A tells the main thread to reconnect; q the end.
 */
static int SDLCALL mousereader(void *arg)
{
    char buf[64], *p;
    Raw r;
    int n, fd;

    (void)arg;
    for (;;) {
        n = read(p9.mousefd, buf, 49);
        if (n <= 0) {
            int again, rfd = p9.mousefd;
            char why[128];

            p9draw_errstr(why, sizeof why);
            again = p9draw_again();
            fd = again < 0 ? -1 : open("/dev/mouse", O_RDWR);
            if (debug()) {
                fprintf(stderr, "sdl plan9: pid %d: mouse read %d on %d (%s), the window again: %d, /dev/mouse %d\n",
                        (int)getpid(), n, rfd, why, again, fd);
            }
            if (fd < 0) {
                break;
            }
            close(p9.mousefd);
            p9.mousefd = fd;
            /* the old keyboard's reader closes its own when its read fails */
            p9.kbdfd = open("/dev/kbd", O_RDONLY);
            startkbd();
            SDL_zero(r);
            r.kind = 'A';
            push(&r);
            continue;
        }
        buf[n] = 0;
        SDL_zero(r);
        r.kind = buf[0];
        if (r.kind != 'm' && r.kind != 'r') {
            continue;
        }
        p = buf + 1;
        r.x = (int)strtol(p, &p, 10);
        r.y = (int)strtol(p, &p, 10);
        r.b = (int)strtol(p, &p, 10);
        push(&r);
    }
    SDL_zero(r);
    r.kind = 'q';
    push(&r);
    return 0;
}

/* kbd(3): messages k, K, c and a UTF-8 string each, NUL after each; arg: the descriptor */
static int SDLCALL kbdreader(void *arg)
{
    char buf[512];
    Raw r;
    int n, i, len, fd;

    fd = (int)(intptr_t)arg;
    for (;;) {
        n = read(fd, buf, sizeof buf - 1);
        if (n <= 0) {
            close(fd);  /* its own: no other reader gets its number meanwhile */
            break;
        }
        buf[n] = 0;
        for (i = 0; i < n; i += len + 1) {
            len = (int)strlen(buf + i);
            if (len == 0) {
                continue;
            }
            SDL_zero(r);
            r.kind = buf[i];
            SDL_strlcpy(r.text, buf + i + 1, sizeof r.text);
            push(&r);
        }
    }
    return 0;
}

static SDL_Scancode scancode(Uint32 r)
{
    if (r >= 'A' && r <= 'Z') {
        r += 'a' - 'A';
    }
    if (r >= 'a' && r <= 'z') {
        return (SDL_Scancode)(SDL_SCANCODE_A + (r - 'a'));
    }
    if (r >= '1' && r <= '9') {
        return (SDL_Scancode)(SDL_SCANCODE_1 + (r - '1'));
    }
    if (r >= KF + 1 && r <= KF + 12) {
        return (SDL_Scancode)(SDL_SCANCODE_F1 + (r - KF - 1));
    }
    switch (r) {
    case '0': return SDL_SCANCODE_0;
    case '\n': return SDL_SCANCODE_RETURN;
    case '\t': return SDL_SCANCODE_TAB;
    case ' ': return SDL_SCANCODE_SPACE;
    case '-': return SDL_SCANCODE_MINUS;
    case '=': return SDL_SCANCODE_EQUALS;
    case '[': return SDL_SCANCODE_LEFTBRACKET;
    case ']': return SDL_SCANCODE_RIGHTBRACKET;
    case '\\': return SDL_SCANCODE_BACKSLASH;
    case ';': return SDL_SCANCODE_SEMICOLON;
    case '\'': return SDL_SCANCODE_APOSTROPHE;
    case '`': return SDL_SCANCODE_GRAVE;
    case ',': return SDL_SCANCODE_COMMA;
    case '.': return SDL_SCANCODE_PERIOD;
    case '/': return SDL_SCANCODE_SLASH;
    case Kbs: return SDL_SCANCODE_BACKSPACE;
    case Kesc: return SDL_SCANCODE_ESCAPE;
    case Kdel: return SDL_SCANCODE_DELETE;
    case Kup: return SDL_SCANCODE_UP;
    case Kdown: return SDL_SCANCODE_DOWN;
    case Kleft: return SDL_SCANCODE_LEFT;
    case Kright: return SDL_SCANCODE_RIGHT;
    case Khome: return SDL_SCANCODE_HOME;
    case Kend: return SDL_SCANCODE_END;
    case Kpgup: return SDL_SCANCODE_PAGEUP;
    case Kpgdown: return SDL_SCANCODE_PAGEDOWN;
    case Kins: return SDL_SCANCODE_INSERT;
    case Kprint: return SDL_SCANCODE_PRINTSCREEN;
    case Kscroll: return SDL_SCANCODE_SCROLLLOCK;
    case Kbreak: return SDL_SCANCODE_PAUSE;
    case Kcaps: return SDL_SCANCODE_CAPSLOCK;
    case Knum: return SDL_SCANCODE_NUMLOCKCLEAR;
    case Kshift: return SDL_SCANCODE_LSHIFT;
    case Kctl: return SDL_SCANCODE_LCTRL;
    case Kalt: return SDL_SCANCODE_LALT;
    case Kaltgr: return SDL_SCANCODE_RALT;
    case Kmod4: return SDL_SCANCODE_LGUI;
    }
    return SDL_SCANCODE_UNKNOWN;
}

/* the keys a k/K message lists */
static int keyset(const char *s, Uint32 *keys)
{
    int n = 0;

    while (*s && n < Nkeys) {
        keys[n++] = SDL_StepUTF8(&s, NULL);
    }
    return n;
}

static bool inset(Uint32 k, const Uint32 *keys, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        if (keys[i] == k) {
            return true;
        }
    }
    return false;
}

/* the keys down now: an event for each that went down or up */
static void keys(const char *s)
{
    Uint32 now[Nkeys];
    int n, i;

    n = keyset(s, now);
    for (i = 0; i < p9.nkeys; i++) {
        if (!inset(p9.keys[i], now, n)) {
            SDL_SendKeyboardKey(0, SDL_DEFAULT_KEYBOARD_ID, (int)p9.keys[i], scancode(p9.keys[i]), false);
        }
    }
    for (i = 0; i < n; i++) {
        if (!inset(now[i], p9.keys, p9.nkeys)) {
            SDL_SendKeyboardKey(0, SDL_DEFAULT_KEYBOARD_ID, (int)now[i], scancode(now[i]), true);
        }
    }
    SDL_memcpy(p9.keys, now, sizeof now[0] * n);
    p9.nkeys = n;
}

/* where the window is now (after r): its size to SDL if it changed */
static void relook(void)
{
    int x, y, w, h;

    p9draw_attach();
    p9draw_rect(&x, &y, &w, &h);
    p9.wx = x;
    p9.wy = y;
    if (p9.window && (w != p9.ww || h != p9.wh)) {
        p9.ww = w;
        p9.wh = h;
        SDL_SendWindowEvent(p9.window, SDL_EVENT_WINDOW_RESIZED, w, h);
    }
    p9.ww = w;
    p9.wh = h;
}

static void PLAN9_PumpEvents(SDL_VideoDevice *_this)
{
    static const Uint8 button[3] = { SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT };
    Raw r;
    int i, changed;

    (void)_this;
    for (;;) {
        SDL_LockMutex(p9.lock);
        if (p9.tail == p9.head) {
            SDL_UnlockMutex(p9.lock);
            break;
        }
        r = p9.raw[p9.tail];
        p9.tail = (p9.tail + 1) % Nraw;
        SDL_UnlockMutex(p9.lock);
        if (!p9.window) {
            continue;
        }
        switch (r.kind) {
        case 'A':
            /* the window in a new wsys: draw(3) anew, everything drawn again */
            p9draw_reconnect();
            p9.ww = p9.wh = -1;
            relook();
            SDL_SendWindowEvent(p9.window, SDL_EVENT_WINDOW_EXPOSED, 0, 0);
            break;
        case 'r':
            relook();
            SDL_FALLTHROUGH;
        case 'm':
            SDL_SendMouseMotion(0, p9.window, SDL_DEFAULT_MOUSE_ID, false, (float)(r.x - p9.wx), (float)(r.y - p9.wy));
            changed = r.b ^ p9.buttons;
            for (i = 0; i < 3; i++) {
                if (changed & (1 << i)) {
                    SDL_SendMouseButton(0, p9.window, SDL_DEFAULT_MOUSE_ID, button[i], (r.b & (1 << i)) != 0);
                }
            }
            /* buttons 4 and 5: the wheel, one step each press */
            if ((changed & 8) && (r.b & 8)) {
                SDL_SendMouseWheel(0, p9.window, SDL_DEFAULT_MOUSE_ID, 0, 1, SDL_MOUSEWHEEL_NORMAL);
            }
            if ((changed & 16) && (r.b & 16)) {
                SDL_SendMouseWheel(0, p9.window, SDL_DEFAULT_MOUSE_ID, 0, -1, SDL_MOUSEWHEEL_NORMAL);
            }
            p9.buttons = r.b;
            break;
        case 'k':
        case 'K':
            keys(r.text);
            break;
        case 'c': {
            const char *s = r.text;
            Uint32 c = SDL_StepUTF8(&s, NULL);

            /* characters, not the keys' private runes or control characters */
            if (c >= 0x20 && c != Kdel && (c < KF || c >= 0xF900)) {
                SDL_SendKeyboardText(r.text);
            }
            break;
        }
        case 'q':
            SDL_SendQuit();
            break;
        }
    }
}

/* the window's size by its wctl (with the border); none without a window system */
static void wctlsize(int w, int h)
{
    char msg[64];
    int fd;

    if ((fd = open("/dev/wctl", O_WRONLY)) < 0) {
        return;
    }
    SDL_snprintf(msg, sizeof msg, "resize -dx %d -dy %d", w + 2 * Borderwidth, h + 2 * Borderwidth);
    write(fd, msg, SDL_strlen(msg));
    close(fd);
}

/*
 * fullscreen: wsys' wctl fullscreen (all of the screen, no border or title
 * bar, above its bars) and unfullscreen (the frame back); rio has neither
 * (its window stays as it is). The new size comes as a resize ('r').
 */
static SDL_FullscreenResult PLAN9_SetWindowFullscreen(SDL_VideoDevice *_this, SDL_Window *window, SDL_VideoDisplay *display, SDL_FullscreenOp fullscreen)
{
    const char *msg = fullscreen == SDL_FULLSCREEN_OP_LEAVE ? "unfullscreen" : "fullscreen";
    int fd, n;

    (void)_this;
    (void)window;
    (void)display;
    if ((fd = open("/dev/wctl", O_WRONLY)) < 0) {
        return SDL_FULLSCREEN_FAILED;
    }
    n = write(fd, msg, SDL_strlen(msg));
    close(fd);
    if (n != (int)SDL_strlen(msg)) {
        return SDL_FULLSCREEN_FAILED;
    }
    relook();
    return SDL_FULLSCREEN_SUCCEEDED;
}

static void PLAN9_SetWindowTitle(SDL_VideoDevice *_this, SDL_Window *window)
{
    const char *t = window->title ? window->title : "";
    int fd;

    (void)_this;
    if ((fd = open("/dev/label", O_WRONLY | O_TRUNC)) >= 0) {
        write(fd, t, SDL_strlen(t));
        close(fd);
    }
}

static bool PLAN9_CreateWindow(SDL_VideoDevice *_this, SDL_Window *window, SDL_PropertiesID props)
{
    (void)_this;
    (void)props;
    if (p9.window) {
        return SDL_SetError("plan9: one window a process (its window of the window system)");
    }
    if (p9.mousefd < 0 && (p9.mousefd = open("/dev/mouse", O_RDWR)) < 0) {
        return SDL_SetError("plan9: /dev/mouse: %s", strerror(errno));
    }
    if (p9.kbdfd < 0) {
        p9.kbdfd = open("/dev/kbd", O_RDONLY);
    }
    if (!(window->flags & SDL_WINDOW_FULLSCREEN)) {
        wctlsize(window->w, window->h);
    } else {
        PLAN9_SetWindowFullscreen(_this, window, NULL, SDL_FULLSCREEN_OP_ENTER);
    }
    p9.window = window;
    relook();
    window->x = p9.wx;
    window->y = p9.wy;
    window->w = p9.ww;
    window->h = p9.wh;
    if (window->title) {
        PLAN9_SetWindowTitle(_this, window);
    }
    /*
     * the readers once a process: SDL makes the window anew now and then
     * (destroy, create), and readers of a window before would read on, and
     * each of them make the window again when its wsys is gone
     */
    if (!p9.readers) {
        p9.readers = true;
        SDL_DetachThread(SDL_CreateThread(mousereader, "plan9 mouse", NULL));
        startkbd();
    }
    SDL_SetMouseFocus(window);
    SDL_SetKeyboardFocus(window);
    return true;
}

static void PLAN9_SetWindowSize(SDL_VideoDevice *_this, SDL_Window *window)
{
    (void)_this;
    wctlsize(window->pending.w, window->pending.h);
    relook();
}

static void PLAN9_DestroyWindow(SDL_VideoDevice *_this, SDL_Window *window)
{
    (void)_this;
    if (p9.window == window) {
        p9.window = NULL;   /* the readers go with the process */
    }
}

static bool PLAN9_CreateWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window, SDL_PixelFormat *format, void **pixels, int *pitch)
{
    int w, h;

    (void)_this;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    SDL_free(p9.fb);
    if (!(p9.fb = SDL_calloc(1, (size_t)w * h * 4))) {
        return false;
    }
    *format = SDL_PIXELFORMAT_XRGB8888;
    *pixels = p9.fb;
    *pitch = w * 4;
    return true;
}

static bool PLAN9_UpdateWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window, const SDL_Rect *rects, int numrects)
{
    int w, h;

    (void)_this;
    (void)rects;
    (void)numrects;
    SDL_GetWindowSizeInPixels(window, &w, &h);
    if (p9draw_put(p9.fb, w, h, w * 4) < 0) {
        return SDL_SetError("plan9: draw: the window's pixels");
    }
    return true;
}

static void PLAN9_DestroyWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window)
{
    (void)_this;
    (void)window;
    SDL_free(p9.fb);
    p9.fb = NULL;
}

static bool PLAN9_SetClipboardText(SDL_VideoDevice *_this, const char *text)
{
    int fd;

    (void)_this;
    if ((fd = open("/dev/snarf", O_WRONLY | O_TRUNC)) < 0) {
        return SDL_SetError("plan9: /dev/snarf: %s", strerror(errno));
    }
    write(fd, text, SDL_strlen(text));
    close(fd);
    return true;
}

static char *PLAN9_GetClipboardText(SDL_VideoDevice *_this)
{
    char *s = NULL, *t;
    size_t n = 0;
    ssize_t m;
    int fd;

    (void)_this;
    if ((fd = open("/dev/snarf", O_RDONLY)) < 0) {
        return SDL_strdup("");
    }
    for (;;) {
        if (!(t = SDL_realloc(s, n + 8192 + 1))) {
            break;
        }
        s = t;
        if ((m = read(fd, s + n, 8192)) <= 0) {
            break;
        }
        n += (size_t)m;
    }
    close(fd);
    if (!s) {
        return SDL_strdup("");
    }
    s[n] = 0;
    return s;
}

static bool PLAN9_HasClipboardText(SDL_VideoDevice *_this)
{
    char *s = PLAN9_GetClipboardText(_this);
    bool has = s && *s;

    SDL_free(s);
    return has;
}

/* the pointer to x y in the window: m x y on the screen to /dev/mouse */
static bool PLAN9_WarpMouse(SDL_Window *window, float x, float y)
{
    char msg[64];
    int n;

    (void)window;
    if (p9.mousefd < 0) {
        return SDL_SetError("plan9: no /dev/mouse");
    }
    n = SDL_snprintf(msg, sizeof msg, "m%d %d", p9.wx + (int)x, p9.wy + (int)y);
    if (write(p9.mousefd, msg, n) != n) {
        return SDL_SetError("plan9: /dev/mouse: %s", strerror(errno));
    }
    return true;
}

static bool PLAN9_VideoInit(SDL_VideoDevice *_this)
{
    SDL_DisplayMode mode;
    int w, h;

    (void)_this;
    if (!(p9.lock = SDL_CreateMutex())) {
        return false;
    }
    if (p9draw_init() < 0) {
        return SDL_SetError("plan9: no draw(3) display");
    }
    p9draw_screen(&w, &h);
    SDL_zero(mode);
    mode.format = SDL_PIXELFORMAT_XRGB8888;
    mode.w = w;
    mode.h = h;
    if (SDL_AddBasicVideoDisplay(&mode) == 0) {
        return false;
    }
    SDL_AddKeyboard(SDL_DEFAULT_KEYBOARD_ID, "plan9 kbd");
    SDL_AddMouse(SDL_DEFAULT_MOUSE_ID, "plan9 mouse");
    PLAN9_InitMouse();
    SDL_GetMouse()->WarpMouse = PLAN9_WarpMouse;
    return true;
}

static void PLAN9_VideoQuit(SDL_VideoDevice *_this)
{
    (void)_this;
    PLAN9_QuitMouse();
    p9draw_close();
}

static void PLAN9_DeleteDevice(SDL_VideoDevice *device)
{
    SDL_free(device);
}

static SDL_VideoDevice *PLAN9_CreateDevice(void)
{
    SDL_VideoDevice *device;

    /* draw(3) there: a 9front terminal */
    if (access("/dev/draw/new", F_OK) < 0) {
        return NULL;
    }
    if (!(device = (SDL_VideoDevice *)SDL_calloc(1, sizeof(SDL_VideoDevice)))) {
        return NULL;
    }
    device->VideoInit = PLAN9_VideoInit;
    device->VideoQuit = PLAN9_VideoQuit;
    device->PumpEvents = PLAN9_PumpEvents;
    device->CreateSDLWindow = PLAN9_CreateWindow;
    device->SetWindowTitle = PLAN9_SetWindowTitle;
    device->SetWindowSize = PLAN9_SetWindowSize;
    device->SetWindowFullscreen = PLAN9_SetWindowFullscreen;
    device->DestroyWindow = PLAN9_DestroyWindow;
    device->CreateWindowFramebuffer = PLAN9_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = PLAN9_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = PLAN9_DestroyWindowFramebuffer;
    device->SetClipboardText = PLAN9_SetClipboardText;
    device->GetClipboardText = PLAN9_GetClipboardText;
    device->HasClipboardText = PLAN9_HasClipboardText;
#ifdef SDL_VIDEO_VULKAN
#if defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES2)
    device->GL_LoadLibrary = PLAN9_GL_LoadLibrary;
    device->GL_GetProcAddress = PLAN9_GL_GetProcAddress;
    device->GL_UnloadLibrary = PLAN9_GL_UnloadLibrary;
    device->GL_CreateContext = PLAN9_GL_CreateContext;
    device->GL_MakeCurrent = PLAN9_GL_MakeCurrent;
    device->GL_SetSwapInterval = PLAN9_GL_SetSwapInterval;
    device->GL_GetSwapInterval = PLAN9_GL_GetSwapInterval;
    device->GL_SwapWindow = PLAN9_GL_SwapWindow;
    device->GL_DestroyContext = PLAN9_GL_DestroyContext;
#endif
    device->Vulkan_LoadLibrary = PLAN9_Vulkan_LoadLibrary;
    device->Vulkan_UnloadLibrary = PLAN9_Vulkan_UnloadLibrary;
    device->Vulkan_GetInstanceExtensions = PLAN9_Vulkan_GetInstanceExtensions;
    device->Vulkan_CreateSurface = PLAN9_Vulkan_CreateSurface;
    device->Vulkan_DestroySurface = PLAN9_Vulkan_DestroySurface;
#endif
    device->free = PLAN9_DeleteDevice;
    return device;
}

/* SDL's slot for a driver outside its tree (SDL_VIDEO_DRIVER_PRIVATE): no change to SDL_video.c */
VideoBootStrap PRIVATE_bootstrap = {
    PLAN9_DRIVER_NAME, "SDL video driver for 9front (rio, wsys: draw(3))",
    PLAN9_CreateDevice,
    NULL, /* no message box of its own */
    false
};

#endif /* SDL_VIDEO_DRIVER_PLAN9 */
