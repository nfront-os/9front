//========================================================================
// GLFW 3.5 for 9front - www.glfw.org
// SPDX-License-Identifier: Zlib
//------------------------------------------------------------------------
// The plan9 platform's window: the process's window of wsys or rio (the
// screen without either).
// - size, place, hidden: /dev/wctl (resize, move, hide, unhide, current);
//   reading it says when the window changed, is current or hidden
// - the mouse: /dev/mouse (m x y buttons msec; r when the window changed,
//   looked up again); the pointer warped by writing m x y to it
// - the keyboard: /dev/kbd (kbd(3) as kbdfs(8) gives it: k and K list the
//   keys down, so each change is a key event; c the character typed)
// - the title: /dev/label; cursors: /dev/cursor (plan9_cursor.c)
// - OpenGL and GLES: EGL's plan9 platform, the native window "" (this
//   process's window); Vulkan: VK_PLAN9_surface
// Readers of /dev/mouse, /dev/kbd and /dev/wctl are threads of their own;
// the event functions take what they read.
//========================================================================

#include "internal.h"

#if defined(_GLFW_PLAN9)

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void push(const _GLFWeventPlan9* e)
{
    int next;

    pthread_mutex_lock(&_glfw.plan9.lock);
    next = (_glfw.plan9.head + 1) % _GLFW_PLAN9_EVENTS;
    if (next != _glfw.plan9.tail)
    {
        _glfw.plan9.events[_glfw.plan9.head] = *e;
        _glfw.plan9.head = next;
    }
    pthread_cond_signal(&_glfw.plan9.cond);
    pthread_mutex_unlock(&_glfw.plan9.lock);
}

// m x y buttons msec, 49 bytes a message; r: the window changed
static void* mousereader(void* arg)
{
    _GLFWeventPlan9 e;
    char buf[64];
    char* p;
    int n;

    (void) arg;
    for (;;)
    {
        n = (int) read(_glfw.plan9.mousefd, buf, 49);
        if (n <= 0)
            break;
        buf[n] = 0;
        memset(&e, 0, sizeof(e));
        e.kind = buf[0];
        if (e.kind != 'm' && e.kind != 'r')
            continue;
        p = buf + 1;
        e.x = (int) strtol(p, &p, 10);
        e.y = (int) strtol(p, &p, 10);
        e.buttons = (int) strtol(p, &p, 10);
        push(&e);
    }
    memset(&e, 0, sizeof(e));
    e.kind = 'q';       // the window is gone
    push(&e);
    return NULL;
}

// kbd(3): messages k, K, c and a UTF-8 string each, NUL after each
static void* kbdreader(void* arg)
{
    _GLFWeventPlan9 e;
    char buf[512];
    int n, i, len;

    (void) arg;
    for (;;)
    {
        n = (int) read(_glfw.plan9.kbdfd, buf, sizeof(buf) - 1);
        if (n <= 0)
            break;
        buf[n] = 0;
        for (i = 0;  i < n;  i += len + 1)
        {
            len = (int) strlen(buf + i);
            if (len == 0)
                continue;
            memset(&e, 0, sizeof(e));
            e.kind = buf[i];
            snprintf(e.text, sizeof(e.text), "%s", buf + i + 1);
            push(&e);
        }
    }
    return NULL;
}

// wctl: a line when the window changed (place, current, hidden)
static void* wctlreader(void* arg)
{
    _GLFWeventPlan9 e;
    int n;

    (void) arg;
    for (;;)
    {
        memset(&e, 0, sizeof(e));
        n = (int) read(_glfw.plan9.wctlfd, e.text, sizeof(e.text) - 1);
        if (n <= 0)
            break;
        e.kind = 'w';
        push(&e);
    }
    return NULL;
}

static void wctl(const char* fmt, ...)
{
    char msg[128];
    va_list arg;
    int fd, n;

    if ((fd = open("/dev/wctl", O_WRONLY)) < 0)
        return;     // no window system: the screen stays as it is
    va_start(arg, fmt);
    n = vsnprintf(msg, sizeof(msg), fmt, arg);
    va_end(arg);
    write(fd, msg, n);
    close(fd);
}

// where the window is now: GLFW hears of a new place or size
static void relook(_GLFWwindow* window)
{
    int x, y, w, h;

    _glfwDrawAttachPlan9();
    _glfwDrawRectPlan9(&x, &y, &w, &h, &_glfw.plan9.border);
    if (x != window->plan9.xpos || y != window->plan9.ypos)
    {
        window->plan9.xpos = x;
        window->plan9.ypos = y;
        _glfwInputWindowPos(window, x, y);
    }
    if (w != window->plan9.width || h != window->plan9.height)
    {
        window->plan9.width = w;
        window->plan9.height = h;
        _glfwInputWindowSize(window, w, h);
        _glfwInputFramebufferSize(window, w, h);
    }
}

// the cursor the window shows now: its own, the arrow, none
void _glfwShowCursorPlan9(_GLFWwindow* window)
{
    static const unsigned char none[72];
    const unsigned char* c;

    if (_glfw.plan9.cursorfd < 0 && (_glfw.plan9.cursorfd = open("/dev/cursor", O_WRONLY)) < 0)
        return;
    if (window->cursorMode == GLFW_CURSOR_HIDDEN || window->cursorMode == GLFW_CURSOR_DISABLED)
        c = none;
    else if (window->cursor && !window->cursor->plan9.arrow)
        c = window->cursor->plan9.c;
    else
    {
        write(_glfw.plan9.cursorfd, "", 0);
        return;
    }
    write(_glfw.plan9.cursorfd, c, 72);
}

static void warp(_GLFWwindow* window, double x, double y)
{
    char msg[64];
    int n;

    n = snprintf(msg, sizeof(msg), "m%d %d", window->plan9.xpos + (int) x, window->plan9.ypos + (int) y);
    write(_glfw.plan9.mousefd, msg, n);
    window->plan9.lastx = (int) x;
    window->plan9.lasty = (int) y;
}

static void center(_GLFWwindow* window)
{
    warp(window, window->plan9.width / 2, window->plan9.height / 2);
}

static void mouse(_GLFWwindow* window, const _GLFWeventPlan9* e)
{
    static const int button[3] = { GLFW_MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_MIDDLE, GLFW_MOUSE_BUTTON_RIGHT };
    const int mods = _glfwModsPlan9();
    const double x = e->x - window->plan9.xpos, y = e->y - window->plan9.ypos;
    const int changed = e->buttons ^ _glfw.plan9.buttons;
    GLFWbool in;
    int i;

    in = x >= 0 && y >= 0 && x < window->plan9.width && y < window->plan9.height;
    if (in != window->plan9.hovered)
    {
        window->plan9.hovered = in;
        _glfwInputCursorEnter(window, in);
    }
    if (window->cursorMode == GLFW_CURSOR_DISABLED)
    {
        // where it would be: moved by as much as it moved from the middle, put back there
        if (x != window->plan9.lastx || y != window->plan9.lasty)
        {
            _glfwInputCursorPos(window,
                                window->virtualCursorPosX + x - window->plan9.lastx,
                                window->virtualCursorPosY + y - window->plan9.lasty);
            center(window);
        }
    }
    else if (x != window->plan9.lastx || y != window->plan9.lasty)
    {
        window->plan9.lastx = x;
        window->plan9.lasty = y;
        _glfwInputCursorPos(window, x, y);
    }
    for (i = 0;  i < 3;  i++)
    {
        if (changed & (1 << i))
        {
            _glfwInputMouseClick(window, button[i],
                                 (e->buttons & (1 << i)) ? GLFW_PRESS : GLFW_RELEASE, mods);
        }
    }
    // buttons 4 and 5: the wheel, one step each press
    if ((changed & 8) && (e->buttons & 8))
        _glfwInputScroll(window, 0, 1);
    if ((changed & 16) && (e->buttons & 16))
        _glfwInputScroll(window, 0, -1);
    _glfw.plan9.buttons = e->buttons;
}

static int utf8(const char** s)
{
    const unsigned char* p = (const unsigned char*) *s;
    uint32_t c;
    int n, i;

    if (p[0] < 0x80)
        n = 0, c = p[0];
    else if ((p[0] & 0xe0) == 0xc0)
        n = 1, c = p[0] & 0x1f;
    else if ((p[0] & 0xf0) == 0xe0)
        n = 2, c = p[0] & 0x0f;
    else
        n = 3, c = p[0] & 0x07;
    for (i = 1;  i <= n;  i++)
    {
        if ((p[i] & 0xc0) != 0x80)
        {
            *s += i;
            return 0xfffd;
        }
        c = c << 6 | (p[i] & 0x3f);
    }
    *s += n + 1;
    return (int) c;
}

static GLFWbool inset(uint32_t k, const uint32_t* keys, int n)
{
    int i;

    for (i = 0;  i < n;  i++)
    {
        if (keys[i] == k)
            return GLFW_TRUE;
    }
    return GLFW_FALSE;
}

// the keys down now: an event for each that went down or up
static void keys(_GLFWwindow* window, const char* s)
{
    uint32_t now[_GLFW_PLAN9_KEYS], r;
    int n = 0, i;

    while (*s && n < _GLFW_PLAN9_KEYS)
        now[n++] = (uint32_t) utf8(&s);
    for (i = 0;  i < _glfw.plan9.nkeys;  i++)
    {
        r = _glfw.plan9.keys[i];
        if (!inset(r, now, n))
        {
            // released: its mods without it
            memmove(_glfw.plan9.keys + i, _glfw.plan9.keys + i + 1, (_glfw.plan9.nkeys - i - 1) * sizeof(r));
            _glfw.plan9.nkeys--;
            i--;
            _glfwInputKey(window, _glfwKeyPlan9(r), (int) r, GLFW_RELEASE, _glfwModsPlan9());
        }
    }
    for (i = 0;  i < n;  i++)
    {
        r = now[i];
        if (!inset(r, _glfw.plan9.keys, _glfw.plan9.nkeys))
        {
            _glfw.plan9.keys[_glfw.plan9.nkeys++] = r;
            _glfwInputKey(window, _glfwKeyPlan9(r), (int) r, GLFW_PRESS, _glfwModsPlan9());
        }
    }
}

// wctl's line: minx miny maxx maxy (with the border), current or not, visible or hidden
static void wctlline(_GLFWwindow* window, const char* s)
{
    const GLFWbool focused = strstr(s, "notcurrent") == NULL && strstr(s, "current") != NULL;
    const GLFWbool hidden = strstr(s, "hidden") != NULL;

    relook(window);
    if (focused != _glfw.plan9.focused)
    {
        _glfw.plan9.focused = focused;
        _glfwInputWindowFocus(window, focused);
    }
    if (hidden != _glfw.plan9.hidden)
    {
        _glfw.plan9.hidden = hidden;
        window->plan9.iconified = hidden;
        _glfwInputWindowIconify(window, hidden);
    }
}

static void handle(_GLFWwindow* window, const _GLFWeventPlan9* e)
{
    const char* s;
    int c, mods;

    switch (e->kind)
    {
        case 'r':
            relook(window);
            // fall through: where the pointer is
        case 'm':
            mouse(window, e);
            break;
        case 'k':
        case 'K':
            keys(window, e->text);
            break;
        case 'c':
            s = e->text;
            c = utf8(&s);
            mods = _glfwModsPlan9();
            // characters, not the keys' private runes or control characters
            if (c >= 0x20 && c != 0x7f && (c < 0xF000 || c >= 0xF900))
                _glfwInputChar(window, (uint32_t) c, mods, !(mods & (GLFW_MOD_CONTROL | GLFW_MOD_ALT)));
            break;
        case 'w':
            wctlline(window, e->text);
            break;
        case 'q':
            _glfwInputWindowCloseRequest(window);
            break;
    }
}

GLFWbool _glfwCreateWindowPlan9(_GLFWwindow* window,
                                const _GLFWwndconfig* wndconfig,
                                const _GLFWctxconfig* ctxconfig,
                                const _GLFWfbconfig* fbconfig)
{
    int x, y, w, h, b, sw, sh;

    if (_glfw.plan9.window)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "plan9: one window a process (its window of the window system)");
        return GLFW_FALSE;
    }
    if (_glfw.plan9.mousefd < 0 && (_glfw.plan9.mousefd = open("/dev/mouse", O_RDWR)) < 0)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "plan9: /dev/mouse: %s", strerror(errno));
        return GLFW_FALSE;
    }
    if (_glfw.plan9.kbdfd < 0)
        _glfw.plan9.kbdfd = open("/dev/kbd", O_RDONLY);
    if (_glfw.plan9.wctlfd < 0)
        _glfw.plan9.wctlfd = open("/dev/wctl", O_RDONLY);

    _glfwDrawAttachPlan9();
    _glfwDrawRectPlan9(&x, &y, &w, &h, &b);
    if (window->monitor)
    {
        // the whole screen, the border outside it
        _glfwDrawScreenPlan9(&sw, &sh);
        wctl("resize -r %d %d %d %d", -b, -b, sw + b, sh + b);
    }
    else
        wctl("resize -dx %d -dy %d", wndconfig->width + 2 * b, wndconfig->height + 2 * b);
    _glfwDrawAttachPlan9();
    _glfwDrawRectPlan9(&window->plan9.xpos, &window->plan9.ypos,
                       &window->plan9.width, &window->plan9.height, &_glfw.plan9.border);
    window->plan9.visible = GLFW_TRUE;
    window->plan9.lastx = window->plan9.lasty = -1;
    _glfw.plan9.window = window;
    _glfw.plan9.focused = GLFW_TRUE;
    if (window->title)
        _glfwSetWindowTitlePlan9(window, window->title);

    if (ctxconfig->client != GLFW_NO_API)
    {
        if (ctxconfig->source == GLFW_NATIVE_CONTEXT_API ||
            ctxconfig->source == GLFW_EGL_CONTEXT_API)
        {
            if (!_glfwInitEGL())
                return GLFW_FALSE;
            if (!_glfwCreateContextEGL(window, ctxconfig, fbconfig))
                return GLFW_FALSE;
        }
        else if (ctxconfig->source == GLFW_OSMESA_CONTEXT_API)
        {
            if (!_glfwInitOSMesa())
                return GLFW_FALSE;
            if (!_glfwCreateContextOSMesa(window, ctxconfig, fbconfig))
                return GLFW_FALSE;
        }

        if (!_glfwRefreshContextAttribs(window, ctxconfig))
            return GLFW_FALSE;
    }

    if (!_glfw.plan9.readers)
    {
        _glfw.plan9.readers = GLFW_TRUE;
        pthread_create(&_glfw.plan9.mousereader, NULL, mousereader, NULL);
        if (_glfw.plan9.kbdfd >= 0)
            pthread_create(&_glfw.plan9.kbdreader, NULL, kbdreader, NULL);
        if (_glfw.plan9.wctlfd >= 0)
            pthread_create(&_glfw.plan9.wctlreader, NULL, wctlreader, NULL);
    }
    if (wndconfig->focused || window->monitor)
        wctl("current");
    return GLFW_TRUE;
}

void _glfwDestroyWindowPlan9(_GLFWwindow* window)
{
    if (window->context.destroy)
        window->context.destroy(window);
    if (_glfw.plan9.window == window)
        _glfw.plan9.window = NULL;     // the readers go with the process
}

void _glfwSetWindowTitlePlan9(_GLFWwindow* window, const char* title)
{
    int fd;

    (void) window;
    if ((fd = open("/dev/label", O_WRONLY | O_TRUNC)) >= 0)
    {
        write(fd, title, strlen(title));
        close(fd);
    }
}

void _glfwSetWindowIconPlan9(_GLFWwindow* window, int count, const GLFWimage* images)
{
    (void) window;
    (void) count;
    (void) images;
    _glfwInputError(GLFW_FEATURE_UNAVAILABLE, "plan9: windows have no icon");
}

void _glfwSetWindowMonitorPlan9(_GLFWwindow* window,
                                _GLFWmonitor* monitor,
                                int xpos, int ypos,
                                int width, int height,
                                int refreshRate)
{
    int sw, sh, b = _glfw.plan9.border;

    (void) refreshRate;
    _glfwInputWindowMonitor(window, monitor);
    if (monitor)
    {
        _glfwDrawScreenPlan9(&sw, &sh);
        wctl("resize -r %d %d %d %d", -b, -b, sw + b, sh + b);
    }
    else
        wctl("resize -r %d %d %d %d", xpos - b, ypos - b, xpos + width + b, ypos + height + b);
    relook(window);
}

void _glfwGetWindowPosPlan9(_GLFWwindow* window, int* xpos, int* ypos)
{
    if (xpos)
        *xpos = window->plan9.xpos;
    if (ypos)
        *ypos = window->plan9.ypos;
}

void _glfwSetWindowPosPlan9(_GLFWwindow* window, int xpos, int ypos)
{
    wctl("move -minx %d -miny %d", xpos - _glfw.plan9.border, ypos - _glfw.plan9.border);
    relook(window);
}

void _glfwGetWindowSizePlan9(_GLFWwindow* window, int* width, int* height)
{
    if (width)
        *width = window->plan9.width;
    if (height)
        *height = window->plan9.height;
}

void _glfwSetWindowSizePlan9(_GLFWwindow* window, int width, int height)
{
    wctl("resize -dx %d -dy %d", width + 2 * _glfw.plan9.border, height + 2 * _glfw.plan9.border);
    relook(window);
}

void _glfwSetWindowSizeLimitsPlan9(_GLFWwindow* window,
                                   int minwidth, int minheight,
                                   int maxwidth, int maxheight)
{
    int w = window->plan9.width, h = window->plan9.height;

    // no limits in the window system: the size now kept inside them
    if (minwidth != GLFW_DONT_CARE && w < minwidth)
        w = minwidth;
    if (minheight != GLFW_DONT_CARE && h < minheight)
        h = minheight;
    if (maxwidth != GLFW_DONT_CARE && w > maxwidth)
        w = maxwidth;
    if (maxheight != GLFW_DONT_CARE && h > maxheight)
        h = maxheight;
    if (w != window->plan9.width || h != window->plan9.height)
        _glfwSetWindowSizePlan9(window, w, h);
}

void _glfwSetWindowAspectRatioPlan9(_GLFWwindow* window, int n, int d)
{
    (void) window;
    (void) n;
    (void) d;
}

void _glfwGetFramebufferSizePlan9(_GLFWwindow* window, int* width, int* height)
{
    _glfwGetWindowSizePlan9(window, width, height);
}

void _glfwGetWindowFrameSizePlan9(_GLFWwindow* window,
                                  int* left, int* top,
                                  int* right, int* bottom)
{
    const int b = window->monitor ? 0 : _glfw.plan9.border;

    if (left)
        *left = b;
    if (top)
        *top = b;
    if (right)
        *right = b;
    if (bottom)
        *bottom = b;
}

void _glfwGetWindowContentScalePlan9(_GLFWwindow* window, float* xscale, float* yscale)
{
    (void) window;
    if (xscale)
        *xscale = 1.f;
    if (yscale)
        *yscale = 1.f;
}

void _glfwIconifyWindowPlan9(_GLFWwindow* window)
{
    (void) window;
    wctl("hide");
}

void _glfwRestoreWindowPlan9(_GLFWwindow* window)
{
    const int* r = window->plan9.restore;

    if (window->plan9.iconified)
        wctl("unhide");
    else if (window->plan9.maximized)
    {
        window->plan9.maximized = GLFW_FALSE;
        wctl("resize -r %d %d %d %d", r[0], r[1], r[2], r[3]);
        _glfwInputWindowMaximize(window, GLFW_FALSE);
        relook(window);
    }
}

void _glfwMaximizeWindowPlan9(_GLFWwindow* window)
{
    int* r = window->plan9.restore;
    const int b = _glfw.plan9.border;
    int sw, sh;

    if (window->plan9.maximized)
        return;
    r[0] = window->plan9.xpos - b;
    r[1] = window->plan9.ypos - b;
    r[2] = window->plan9.xpos + window->plan9.width + b;
    r[3] = window->plan9.ypos + window->plan9.height + b;
    _glfwDrawScreenPlan9(&sw, &sh);
    wctl("resize -r 0 0 %d %d", sw, sh);
    window->plan9.maximized = GLFW_TRUE;
    _glfwInputWindowMaximize(window, GLFW_TRUE);
    relook(window);
}

GLFWbool _glfwWindowMaximizedPlan9(_GLFWwindow* window)
{
    return window->plan9.maximized;
}

GLFWbool _glfwWindowHoveredPlan9(_GLFWwindow* window)
{
    return window->plan9.hovered;
}

GLFWbool _glfwFramebufferTransparentPlan9(_GLFWwindow* window)
{
    (void) window;
    return GLFW_FALSE;
}

void _glfwSetWindowResizablePlan9(_GLFWwindow* window, GLFWbool enabled)
{
    (void) window;
    (void) enabled;
}

void _glfwSetWindowDecoratedPlan9(_GLFWwindow* window, GLFWbool enabled)
{
    (void) window;
    (void) enabled;
}

void _glfwSetWindowFloatingPlan9(_GLFWwindow* window, GLFWbool enabled)
{
    (void) window;
    (void) enabled;
}

void _glfwSetWindowMousePassthroughPlan9(_GLFWwindow* window, GLFWbool enabled)
{
    (void) window;
    (void) enabled;
}

float _glfwGetWindowOpacityPlan9(_GLFWwindow* window)
{
    (void) window;
    return 1.f;
}

void _glfwSetWindowOpacityPlan9(_GLFWwindow* window, float opacity)
{
    (void) window;
    (void) opacity;
}

void _glfwSetRawMouseMotionPlan9(_GLFWwindow *window, GLFWbool enabled)
{
    (void) window;
    (void) enabled;
}

GLFWbool _glfwRawMouseMotionSupportedPlan9(void)
{
    return GLFW_FALSE;
}

void _glfwShowWindowPlan9(_GLFWwindow* window)
{
    window->plan9.visible = GLFW_TRUE;
    wctl("unhide");
}

void _glfwRequestWindowAttentionPlan9(_GLFWwindow* window)
{
    (void) window;
}

void _glfwHideWindowPlan9(_GLFWwindow* window)
{
    window->plan9.visible = GLFW_FALSE;
    wctl("hide");
}

void _glfwFocusWindowPlan9(_GLFWwindow* window)
{
    (void) window;
    wctl("current");
}

GLFWbool _glfwWindowFocusedPlan9(_GLFWwindow* window)
{
    (void) window;
    return _glfw.plan9.focused;
}

GLFWbool _glfwWindowIconifiedPlan9(_GLFWwindow* window)
{
    return window->plan9.iconified;
}

GLFWbool _glfwWindowVisiblePlan9(_GLFWwindow* window)
{
    return window->plan9.visible;
}

void _glfwPollEventsPlan9(void)
{
    _GLFWeventPlan9 e;

    for (;;)
    {
        pthread_mutex_lock(&_glfw.plan9.lock);
        if (_glfw.plan9.tail == _glfw.plan9.head)
        {
            pthread_mutex_unlock(&_glfw.plan9.lock);
            break;
        }
        e = _glfw.plan9.events[_glfw.plan9.tail];
        _glfw.plan9.tail = (_glfw.plan9.tail + 1) % _GLFW_PLAN9_EVENTS;
        pthread_mutex_unlock(&_glfw.plan9.lock);
        if (_glfw.plan9.window)
            handle(_glfw.plan9.window, &e);
    }
}

// until an event or glfwPostEmptyEvent; until the time, if one is given
static void waitfor(const struct timespec* until)
{
    pthread_mutex_lock(&_glfw.plan9.lock);
    while (_glfw.plan9.tail == _glfw.plan9.head && !_glfw.plan9.wake)
    {
        if (!until)
            pthread_cond_wait(&_glfw.plan9.cond, &_glfw.plan9.lock);
        else if (pthread_cond_timedwait(&_glfw.plan9.cond, &_glfw.plan9.lock, until) != 0)
            break;
    }
    _glfw.plan9.wake = GLFW_FALSE;
    pthread_mutex_unlock(&_glfw.plan9.lock);
    _glfwPollEventsPlan9();
}

void _glfwWaitEventsPlan9(void)
{
    waitfor(NULL);
}

void _glfwWaitEventsTimeoutPlan9(double timeout)
{
    struct timespec t;
    long long ns;

    clock_gettime(CLOCK_MONOTONIC, &t);
    ns = t.tv_nsec + (long long) (timeout * 1e9);
    t.tv_sec += (time_t) (ns / 1000000000);
    t.tv_nsec = (long) (ns % 1000000000);
    waitfor(&t);
}

void _glfwPostEmptyEventPlan9(void)
{
    pthread_mutex_lock(&_glfw.plan9.lock);
    _glfw.plan9.wake = GLFW_TRUE;
    pthread_cond_signal(&_glfw.plan9.cond);
    pthread_mutex_unlock(&_glfw.plan9.lock);
}

void _glfwGetCursorPosPlan9(_GLFWwindow* window, double* xpos, double* ypos)
{
    if (xpos)
        *xpos = window->plan9.lastx;
    if (ypos)
        *ypos = window->plan9.lasty;
}

void _glfwSetCursorPosPlan9(_GLFWwindow* window, double x, double y)
{
    warp(window, x, y);
}

void _glfwSetCursorModePlan9(_GLFWwindow* window, int mode)
{
    if (mode == GLFW_CURSOR_DISABLED)
        center(window);
    _glfwShowCursorPlan9(window);
}

void _glfwSetCursorPlan9(_GLFWwindow* window, _GLFWcursor* cursor)
{
    (void) cursor;      // window->cursor already
    _glfwShowCursorPlan9(window);
}

EGLenum _glfwGetEGLPlatformPlan9(EGLint** attribs)
{
    (void) attribs;
    return 0;   // eglGetDisplay: Mesa's plan9 platform is its default
}

EGLNativeDisplayType _glfwGetEGLNativeDisplayPlan9(void)
{
    return EGL_DEFAULT_DISPLAY;
}

EGLNativeWindowType _glfwGetEGLNativeWindowPlan9(_GLFWwindow* window)
{
    (void) window;
    return (EGLNativeWindowType) "";    // "": this process's window
}

void _glfwGetRequiredInstanceExtensionsPlan9(char** extensions)
{
    if (!_glfw.vk.KHR_surface)
        return;
    extensions[0] = "VK_KHR_surface";
    extensions[1] = "VK_PLAN9_surface";
}

GLFWbool _glfwGetPhysicalDevicePresentationSupportPlan9(VkInstance instance,
                                                        VkPhysicalDevice device,
                                                        uint32_t queuefamily)
{
    (void) instance;
    (void) device;
    (void) queuefamily;
    return GLFW_TRUE;
}

VkResult _glfwCreateWindowSurfacePlan9(VkInstance instance,
                                       _GLFWwindow* window,
                                       const VkAllocationCallbacks* allocator,
                                       VkSurfaceKHR* surface)
{
    PFN_vkCreatePlan9SurfacePLAN9 create;
    VkPlan9SurfaceCreateInfoPLAN9 info;
    VkResult err;

    (void) window;
    create = (PFN_vkCreatePlan9SurfacePLAN9) vkGetInstanceProcAddr(instance, "vkCreatePlan9SurfacePLAN9");
    if (!create)
    {
        _glfwInputError(GLFW_API_UNAVAILABLE, "plan9: Vulkan instance missing VK_PLAN9_surface extension");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
    memset(&info, 0, sizeof(info));
    info.sType = VK_STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9;
    info.pWindowName = NULL;    // the process's window (/dev/winname), else the screen
    err = create(instance, &info, allocator, surface);
    if (err)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "plan9: Failed to create Vulkan surface: %s",
                        _glfwGetVulkanResultString(err));
    }
    return err;
}

#endif // _GLFW_PLAN9
