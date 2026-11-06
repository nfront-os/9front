/*
 * SPDX-License-Identifier: MIT
 *
 * glfwtest: GLFW on 9front (sys/src/glfw, the plan9 platform) in a window
 * of wsys or rio: the platform, the monitor (the screen), a window without
 * a client API (GLFW_NO_API: what Vulkan programs make) of the size asked
 * for, its title (/dev/label), place and border, resized; the clipboard
 * (/dev/snarf); key names; standard cursors and hidden ones (/dev/cursor,
 * read back where wsys gives it); waiting for events with a time and woken
 * by glfwPostEmptyEvent from another thread.  With a GPU (-g): an OpenGL
 * context (EGL, zink), a frame cleared green and read back; Vulkan's
 * extensions, an instance with them and a surface of the window
 * (VK_PLAN9_surface) the device can present to.
 * -e secs: then the input events for secs seconds (keys, the mouse), a line
 * each (the host types and moves the mouse: tests/glfw/run.sh).
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 *
 * usage: glfwtest [-g] [-e secs]
 */
#include <vulkan/vulkan.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { W = 320, H = 240 };

static int nfail;

#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); nfail++; } while(0)

static void error(int code, const char *msg)
{
    printf("glfw error %#x: %s\n", code, msg);
}

static int sized;

static void size(GLFWwindow *w, int width, int height)
{
    (void)w;
    sized = width << 16 | height;
}

/* the window's cursor as mouse(3) gives it: 72 bytes, none for the default; -1 unreadable */
static int readcursor(unsigned char *c)
{
    int fd, n;

    if ((fd = open("/dev/cursor", O_RDONLY)) < 0) {
        return -1;
    }
    n = (int)read(fd, c, 72);
    close(fd);
    return n;
}

static int le32(const unsigned char *p)
{
    return (int)(p[0] | p[1] << 8 | p[2] << 16 | (unsigned)p[3] << 24);
}

static void *poster(void *arg)
{
    (void)arg;
    usleep(300 * 1000);
    glfwPostEmptyEvent();
    return NULL;
}

static void cursors(GLFWwindow *win)
{
    GLFWcursor *cross;
    unsigned char c[72];
    int n, i, bad;

    cross = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    glfwSetCursor(win, cross);
    if ((n = readcursor(c)) < 0) {
        OK("cursor set; /dev/cursor not readable here (rio)");
        glfwSetCursor(win, NULL);
        glfwDestroyCursor(cross);
        return;
    }
    if (n != 72 || le32(c) != -7 || le32(c + 4) != -7 || c[40 + 2 * 7] != 0xf8) {
        FAIL("crosshair cursor: /dev/cursor has %d bytes", n);
    } else {
        OK("crosshair cursor in /dev/cursor");
    }
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    n = readcursor(c);
    for (i = 8, bad = n != 72; i < 72 && !bad; i++) {
        bad = c[i] != 0;
    }
    if (bad) {
        FAIL("hidden cursor: /dev/cursor has %d bytes, not a cursor of no pixels", n);
    } else {
        OK("hidden cursor: no pixels");
    }
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetCursor(win, NULL);
    if ((n = readcursor(c)) != 0) {
        FAIL("default cursor: /dev/cursor has %d bytes, want none (the arrow)", n);
    } else {
        OK("default cursor: the window system's arrow");
    }
    glfwDestroyCursor(cross);
}

/* -e: the input, as GLFW gives it */
static void key(GLFWwindow *w, int k, int scancode, int action, int mods)
{
    const char *name = glfwGetKeyName(k, scancode);

    (void)w;
    printf("event key %s %d %s mods %#x\n", action == GLFW_PRESS ? "down" : action == GLFW_RELEASE ? "up" : "repeat",
           k, name ? name : "-", mods);
    fflush(stdout);
}

static void character(GLFWwindow *w, unsigned c)
{
    (void)w;
    printf("event char %c\n", c < 0x80 ? (char)c : '?');
    fflush(stdout);
}

static void button(GLFWwindow *w, int b, int action, int mods)
{
    double x, y;

    (void)mods;
    glfwGetCursorPos(w, &x, &y);
    printf("event button %d %s at %.0f,%.0f\n", b, action == GLFW_PRESS ? "down" : "up", x, y);
    fflush(stdout);
}

static void motion(GLFWwindow *w, double x, double y)
{
    (void)w;
    printf("event motion %.0f,%.0f\n", x, y);
    fflush(stdout);
}

static void scroll(GLFWwindow *w, double dx, double dy)
{
    (void)w;
    (void)dx;
    printf("event scroll %.0f\n", dy);
    fflush(stdout);
}

/* GL's, by glfwGetProcAddress: no GL headers */
typedef void (*Clear)(unsigned);
typedef void (*ClearColor)(float, float, float, float);
typedef void (*ReadPixels)(int, int, int, int, unsigned, unsigned, void *);
typedef void (*Finish)(void);
typedef const unsigned char *(*GetString)(unsigned);

static void gl(void)
{
    GLFWwindow *win;
    ClearColor clearcolor;
    Clear clear;
    ReadPixels readpixels;
    Finish finish;
    GetString getstring;
    unsigned char p[4];
    int i;

    glfwDefaultWindowHints();
    if (!(win = glfwCreateWindow(W, H, "glfwtest gl", NULL, NULL))) {
        FAIL("OpenGL window");
        return;
    }
    glfwMakeContextCurrent(win);
    clearcolor = (ClearColor)glfwGetProcAddress("glClearColor");
    clear = (Clear)glfwGetProcAddress("glClear");
    readpixels = (ReadPixels)glfwGetProcAddress("glReadPixels");
    finish = (Finish)glfwGetProcAddress("glFinish");
    getstring = (GetString)glfwGetProcAddress("glGetString");
    if (!clearcolor || !clear || !readpixels || !finish || !getstring) {
        FAIL("glfwGetProcAddress");
        glfwDestroyWindow(win);
        return;
    }
    OK("OpenGL context: %s, %s", (const char *)getstring(0x1F02), (const char *)getstring(0x1F01));
    glfwSwapInterval(1);
    for (i = 0; i < 30; i++) {
        clearcolor(0, 1, 0, 1);
        clear(0x4000);
        if (i == 29) {
            finish();
            readpixels(W / 2, H / 2, 1, 1, 0x1908, 0x1401, p);
            if (p[0] != 0 || p[1] != 255 || p[2] != 0) {
                FAIL("OpenGL: glReadPixels %02x%02x%02x, want 00ff00", p[0], p[1], p[2]);
            } else {
                OK("OpenGL: glReadPixels 00ff00");
            }
        }
        glfwSwapBuffers(win);
        glfwPollEvents();
    }
    OK("30 frames swapped");
    glfwDestroyWindow(win);
}

static void vulkan(void)
{
    PFN_vkCreateInstance createinstance;
    PFN_vkDestroyInstance destroyinstance;
    PFN_vkEnumeratePhysicalDevices enumdevices;
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR support;
    PFN_vkDestroySurfaceKHR destroysurface;
    VkInstanceCreateInfo ici;
    VkInstance inst;
    VkPhysicalDevice dev;
    VkSurfaceKHR surf;
    VkBool32 ok;
    VkResult r;
    GLFWwindow *win;
    const char **ext;
    uint32_t n, i;

    if (!glfwVulkanSupported()) {
        FAIL("glfwVulkanSupported: no");
        return;
    }
    ext = glfwGetRequiredInstanceExtensions(&n);
    if (!ext || n != 2) {
        FAIL("glfwGetRequiredInstanceExtensions: %u", ext ? n : 0);
        return;
    }
    for (i = 0; i < n; i++) {
        OK("Vulkan instance extension %s", ext[i]);
    }
    createinstance = (PFN_vkCreateInstance)glfwGetInstanceProcAddress(NULL, "vkCreateInstance");
    memset(&ici, 0, sizeof ici);
    ici.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    ici.enabledExtensionCount = n;
    ici.ppEnabledExtensionNames = ext;
    if (!createinstance || (r = createinstance(&ici, NULL, &inst)) != VK_SUCCESS) {
        FAIL("vkCreateInstance with GLFW's extensions");
        return;
    }
    destroyinstance = (PFN_vkDestroyInstance)glfwGetInstanceProcAddress(inst, "vkDestroyInstance");
    enumdevices = (PFN_vkEnumeratePhysicalDevices)glfwGetInstanceProcAddress(inst, "vkEnumeratePhysicalDevices");
    support = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)glfwGetInstanceProcAddress(inst, "vkGetPhysicalDeviceSurfaceSupportKHR");
    destroysurface = (PFN_vkDestroySurfaceKHR)glfwGetInstanceProcAddress(inst, "vkDestroySurfaceKHR");
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (!(win = glfwCreateWindow(W, H, "glfwtest vulkan", NULL, NULL))) {
        FAIL("Vulkan window");
        destroyinstance(inst, NULL);
        return;
    }
    if ((r = glfwCreateWindowSurface(inst, win, NULL, &surf)) != VK_SUCCESS) {
        FAIL("glfwCreateWindowSurface: %d", r);
    } else {
        n = 1;
        ok = VK_FALSE;
        if (enumdevices(inst, &n, &dev) >= 0 && n == 1 && support(dev, 0, surf, &ok) == VK_SUCCESS && ok &&
            glfwGetPhysicalDevicePresentationSupport(inst, dev, 0)) {
            OK("Vulkan surface of the window (VK_PLAN9_surface), queue family 0 presents to it");
        } else {
            FAIL("Vulkan surface: no device presents to it");
        }
        destroysurface(inst, surf, NULL);
    }
    glfwDestroyWindow(win);
    destroyinstance(inst, NULL);
}

int main(int argc, char **argv)
{
    GLFWwindow *win;
    GLFWmonitor *mon;
    const GLFWvidmode *mode;
    pthread_t t;
    char buf[64];
    const char *s;
    int w, h, x, y, l, top, r, b, fd, n, gpu = 0, evsecs = 0, i;
    double t0, t1;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-g") == 0) {
            gpu = 1;
        } else if (strcmp(argv[i], "-e") == 0 && i + 1 < argc) {
            evsecs = atoi(argv[++i]);
        }
    }
    glfwSetErrorCallback(error);
    if (!glfwInit()) {
        printf("FAIL glfwInit\nglfwtest: failed\n");
        return 1;
    }
    if (glfwGetPlatform() == GLFW_PLATFORM_PLAN9) {
        OK("platform plan9 (%s)", glfwGetVersionString());
    } else {
        FAIL("platform %#x, not plan9", glfwGetPlatform());
    }
    mon = glfwGetPrimaryMonitor();
    mode = mon ? glfwGetVideoMode(mon) : NULL;
    if (!mode || mode->width < 640 || mode->height < 480) {
        FAIL("monitor: %dx%d", mode ? mode->width : 0, mode ? mode->height : 0);
    } else {
        OK("monitor %s %dx%d", glfwGetMonitorName(mon), mode->width, mode->height);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if (!(win = glfwCreateWindow(W, H, "glfwtest", NULL, NULL))) {
        printf("FAIL glfwCreateWindow\nglfwtest: failed\n");
        return 1;
    }
    glfwSetWindowSizeCallback(win, size);
    glfwGetWindowSize(win, &w, &h);
    glfwGetFramebufferSize(win, &x, &y);
    if (w == W && h == H && x == W && y == H) {
        OK("window %dx%d", w, h);
    } else {
        FAIL("window %dx%d, framebuffer %dx%d, want %dx%d", w, h, x, y, W, H);
    }
    if ((fd = open("/dev/label", O_RDONLY)) >= 0 && (n = (int)read(fd, buf, sizeof buf)) == 8 && memcmp(buf, "glfwtest", 8) == 0) {
        OK("title in /dev/label");
    } else {
        FAIL("title not in /dev/label");
    }
    if (fd >= 0) {
        close(fd);
    }
    glfwGetWindowPos(win, &x, &y);
    glfwGetWindowFrameSize(win, &l, &top, &r, &b);
    OK("window at %d,%d, border %d %d %d %d", x, y, l, top, r, b);

    /* resized: GLFW hears of it */
    glfwSetWindowSize(win, W + 40, H + 20);
    t0 = glfwGetTime();
    while (sized != ((W + 40) << 16 | (H + 20)) && glfwGetTime() - t0 < 2) {
        glfwWaitEventsTimeout(0.1);
    }
    glfwGetWindowSize(win, &w, &h);
    if (w == W + 40 && h == H + 20 && sized == (w << 16 | h)) {
        OK("resized to %dx%d, the size callback called", w, h);
    } else {
        FAIL("resized to %dx%d, callback %dx%d, want %dx%d", w, h, sized >> 16, sized & 0xffff, W + 40, H + 20);
    }

    glfwSetClipboardString(win, "glfwtest snarf");
    if ((s = glfwGetClipboardString(win)) && strcmp(s, "glfwtest snarf") == 0) {
        OK("clipboard through /dev/snarf");
    } else {
        FAIL("clipboard: %s", s ? s : "nil");
    }

    s = glfwGetKeyName(GLFW_KEY_A, 0);
    if (s && strcmp(s, "a") == 0 && glfwGetKeyScancode(GLFW_KEY_ESCAPE) == 0x1b && glfwGetKeyName(GLFW_KEY_ESCAPE, 0) == NULL) {
        OK("key names and scancodes (kbd(3)'s runes)");
    } else {
        FAIL("key name of A: %s, scancode of Escape %#x", s ? s : "nil", glfwGetKeyScancode(GLFW_KEY_ESCAPE));
    }

    cursors(win);

    /* what came of the above first (the window's wctl), then nothing comes */
    for (i = 0; i < 5; i++) {
        glfwWaitEventsTimeout(0.05);
    }
    t0 = glfwGetTime();
    glfwWaitEventsTimeout(0.2);
    t1 = glfwGetTime();
    if (t1 - t0 >= 0.15 && t1 - t0 < 1) {
        OK("glfwWaitEventsTimeout(0.2): back after %.2f s", t1 - t0);
    } else {
        FAIL("glfwWaitEventsTimeout(0.2): back after %.2f s", t1 - t0);
    }
    /* woken by another thread: drain what came, then wait */
    glfwPollEvents();
    pthread_create(&t, NULL, poster, NULL);
    t0 = glfwGetTime();
    glfwWaitEvents();
    t1 = glfwGetTime();
    pthread_join(t, NULL);
    if (t1 - t0 < 5) {
        OK("glfwWaitEvents woken by glfwPostEmptyEvent after %.2f s", t1 - t0);
    } else {
        FAIL("glfwWaitEvents: %.2f s", t1 - t0);
    }

    if (evsecs > 0) {
        glfwSetKeyCallback(win, key);
        glfwSetCharCallback(win, character);
        glfwSetMouseButtonCallback(win, button);
        glfwSetCursorPosCallback(win, motion);
        glfwSetScrollCallback(win, scroll);
        printf("ready for events\n");
        fflush(stdout);
        t0 = glfwGetTime();
        while (glfwGetTime() - t0 < evsecs && !glfwWindowShouldClose(win)) {
            glfwWaitEventsTimeout(0.1);
        }
    }
    glfwDestroyWindow(win);

    if (gpu) {
        gl();
        vulkan();
    }
    glfwTerminate();
    printf(nfail ? "glfwtest: %d failed\n" : "glfwtest: all passed\n", nfail);
    return nfail != 0;
}
