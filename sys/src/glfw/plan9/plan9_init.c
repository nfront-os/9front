//========================================================================
// GLFW 3.5 for 9front - www.glfw.org
// SPDX-License-Identifier: Zlib
//------------------------------------------------------------------------
// The plan9 platform: its functions for GLFW, the keys (kbd(3)'s runes),
// the clipboard (/dev/snarf).
//========================================================================

#include "internal.h"

#if defined(_GLFW_PLAN9)

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Plan 9's keyboard.h: its runes for keys that are not characters
enum
{
    KF = 0xF000, Spec = 0xF800,
    Khome = KF|0x0D, Kup = KF|0x0E, Kdown = Spec|0x00, Kpgup = KF|0x0F, Kprint = KF|0x10,
    Kleft = KF|0x11, Kright = KF|0x12, Kpgdown = KF|0x13, Kins = KF|0x14, Kalt = KF|0x15,
    Kshift = KF|0x16, Kctl = KF|0x17, Kend = KF|0x18, Kscroll = KF|0x19,
    Kbs = 0x08, Kdel = 0x7f, Kesc = 0x1b,
    Kbreak = Spec|0x61, Kcaps = Spec|0x64, Knum = Spec|0x65, Kaltgr = Spec|0x67, Kmod4 = Spec|0x68,
};

static const struct
{
    uint32_t rune;
    int key;
} keymap[] =
{
    { ' ', GLFW_KEY_SPACE }, { '\'', GLFW_KEY_APOSTROPHE }, { ',', GLFW_KEY_COMMA },
    { '-', GLFW_KEY_MINUS }, { '.', GLFW_KEY_PERIOD }, { '/', GLFW_KEY_SLASH },
    { ';', GLFW_KEY_SEMICOLON }, { '=', GLFW_KEY_EQUAL }, { '[', GLFW_KEY_LEFT_BRACKET },
    { '\\', GLFW_KEY_BACKSLASH }, { ']', GLFW_KEY_RIGHT_BRACKET }, { '`', GLFW_KEY_GRAVE_ACCENT },
    { Kesc, GLFW_KEY_ESCAPE }, { '\n', GLFW_KEY_ENTER }, { '\t', GLFW_KEY_TAB },
    { Kbs, GLFW_KEY_BACKSPACE }, { Kins, GLFW_KEY_INSERT }, { Kdel, GLFW_KEY_DELETE },
    { Kright, GLFW_KEY_RIGHT }, { Kleft, GLFW_KEY_LEFT }, { Kdown, GLFW_KEY_DOWN },
    { Kup, GLFW_KEY_UP }, { Kpgup, GLFW_KEY_PAGE_UP }, { Kpgdown, GLFW_KEY_PAGE_DOWN },
    { Khome, GLFW_KEY_HOME }, { Kend, GLFW_KEY_END }, { Kcaps, GLFW_KEY_CAPS_LOCK },
    { Kscroll, GLFW_KEY_SCROLL_LOCK }, { Knum, GLFW_KEY_NUM_LOCK }, { Kprint, GLFW_KEY_PRINT_SCREEN },
    { Kbreak, GLFW_KEY_PAUSE }, { Kshift, GLFW_KEY_LEFT_SHIFT }, { Kctl, GLFW_KEY_LEFT_CONTROL },
    { Kalt, GLFW_KEY_LEFT_ALT }, { Kmod4, GLFW_KEY_LEFT_SUPER }, { Kaltgr, GLFW_KEY_RIGHT_ALT },
};

// a rune of kbd(3)'s k and K messages: its key
int _glfwKeyPlan9(uint32_t r)
{
    size_t i;

    if (r >= 'A' && r <= 'Z')
        r += 'a' - 'A';
    if (r >= 'a' && r <= 'z')
        return GLFW_KEY_A + (int) (r - 'a');
    if (r >= '0' && r <= '9')
        return GLFW_KEY_0 + (int) (r - '0');
    if (r >= KF + 1 && r <= KF + 12)
        return GLFW_KEY_F1 + (int) (r - KF - 1);
    for (i = 0;  i < sizeof(keymap) / sizeof(keymap[0]);  i++)
    {
        if (keymap[i].rune == r)
            return keymap[i].key;
    }
    return GLFW_KEY_UNKNOWN;
}

// the modifier keys among the keys down
int _glfwModsPlan9(void)
{
    int i, mods = 0;

    for (i = 0;  i < _glfw.plan9.nkeys;  i++)
    {
        switch (_glfw.plan9.keys[i])
        {
            case Kshift: mods |= GLFW_MOD_SHIFT; break;
            case Kctl: mods |= GLFW_MOD_CONTROL; break;
            case Kalt: case Kaltgr: mods |= GLFW_MOD_ALT; break;
            case Kmod4: mods |= GLFW_MOD_SUPER; break;
            case Kcaps: mods |= GLFW_MOD_CAPS_LOCK; break;
            case Knum: mods |= GLFW_MOD_NUM_LOCK; break;
        }
    }
    return mods;
}

GLFWbool _glfwConnectPlan9(int platformID, _GLFWplatform* platform)
{
    const _GLFWplatform plan9 =
    {
        .platformID = GLFW_PLATFORM_PLAN9,
        .init = _glfwInitPlan9,
        .terminate = _glfwTerminatePlan9,
        .getCursorPos = _glfwGetCursorPosPlan9,
        .setCursorPos = _glfwSetCursorPosPlan9,
        .setCursorMode = _glfwSetCursorModePlan9,
        .setRawMouseMotion = _glfwSetRawMouseMotionPlan9,
        .rawMouseMotionSupported = _glfwRawMouseMotionSupportedPlan9,
        .createCursor = _glfwCreateCursorPlan9,
        .createStandardCursor = _glfwCreateStandardCursorPlan9,
        .destroyCursor = _glfwDestroyCursorPlan9,
        .setCursor = _glfwSetCursorPlan9,
        .getScancodeName = _glfwGetScancodeNamePlan9,
        .getKeyScancode = _glfwGetKeyScancodePlan9,
        .setClipboardString = _glfwSetClipboardStringPlan9,
        .getClipboardString = _glfwGetClipboardStringPlan9,
        .initJoysticks = _glfwInitJoysticksNull,
        .terminateJoysticks = _glfwTerminateJoysticksNull,
        .pollJoystick = _glfwPollJoystickNull,
        .getMappingName = _glfwGetMappingNameNull,
        .updateGamepadGUID = _glfwUpdateGamepadGUIDNull,
        .freeMonitor = _glfwFreeMonitorPlan9,
        .getMonitorPos = _glfwGetMonitorPosPlan9,
        .getMonitorContentScale = _glfwGetMonitorContentScalePlan9,
        .getMonitorWorkarea = _glfwGetMonitorWorkareaPlan9,
        .getVideoModes = _glfwGetVideoModesPlan9,
        .getVideoMode = _glfwGetVideoModePlan9,
        .getGammaRamp = _glfwGetGammaRampPlan9,
        .setGammaRamp = _glfwSetGammaRampPlan9,
        .createWindow = _glfwCreateWindowPlan9,
        .destroyWindow = _glfwDestroyWindowPlan9,
        .setWindowTitle = _glfwSetWindowTitlePlan9,
        .setWindowIcon = _glfwSetWindowIconPlan9,
        .getWindowPos = _glfwGetWindowPosPlan9,
        .setWindowPos = _glfwSetWindowPosPlan9,
        .getWindowSize = _glfwGetWindowSizePlan9,
        .setWindowSize = _glfwSetWindowSizePlan9,
        .setWindowSizeLimits = _glfwSetWindowSizeLimitsPlan9,
        .setWindowAspectRatio = _glfwSetWindowAspectRatioPlan9,
        .getFramebufferSize = _glfwGetFramebufferSizePlan9,
        .getWindowFrameSize = _glfwGetWindowFrameSizePlan9,
        .getWindowContentScale = _glfwGetWindowContentScalePlan9,
        .iconifyWindow = _glfwIconifyWindowPlan9,
        .restoreWindow = _glfwRestoreWindowPlan9,
        .maximizeWindow = _glfwMaximizeWindowPlan9,
        .showWindow = _glfwShowWindowPlan9,
        .hideWindow = _glfwHideWindowPlan9,
        .requestWindowAttention = _glfwRequestWindowAttentionPlan9,
        .focusWindow = _glfwFocusWindowPlan9,
        .setWindowMonitor = _glfwSetWindowMonitorPlan9,
        .windowFocused = _glfwWindowFocusedPlan9,
        .windowIconified = _glfwWindowIconifiedPlan9,
        .windowVisible = _glfwWindowVisiblePlan9,
        .windowMaximized = _glfwWindowMaximizedPlan9,
        .windowHovered = _glfwWindowHoveredPlan9,
        .framebufferTransparent = _glfwFramebufferTransparentPlan9,
        .getWindowOpacity = _glfwGetWindowOpacityPlan9,
        .setWindowResizable = _glfwSetWindowResizablePlan9,
        .setWindowDecorated = _glfwSetWindowDecoratedPlan9,
        .setWindowFloating = _glfwSetWindowFloatingPlan9,
        .setWindowOpacity = _glfwSetWindowOpacityPlan9,
        .setWindowMousePassthrough = _glfwSetWindowMousePassthroughPlan9,
        .pollEvents = _glfwPollEventsPlan9,
        .waitEvents = _glfwWaitEventsPlan9,
        .waitEventsTimeout = _glfwWaitEventsTimeoutPlan9,
        .postEmptyEvent = _glfwPostEmptyEventPlan9,
        .getEGLPlatform = _glfwGetEGLPlatformPlan9,
        .getEGLNativeDisplay = _glfwGetEGLNativeDisplayPlan9,
        .getEGLNativeWindow = _glfwGetEGLNativeWindowPlan9,
        .getRequiredInstanceExtensions = _glfwGetRequiredInstanceExtensionsPlan9,
        .getPhysicalDevicePresentationSupport = _glfwGetPhysicalDevicePresentationSupportPlan9,
        .createWindowSurface = _glfwCreateWindowSurfacePlan9
    };

    (void) platformID;
    // draw(3) there: a 9front terminal
    if (access("/dev/draw/new", F_OK) < 0)
    {
        if (platformID == GLFW_PLATFORM_PLAN9)
            _glfwInputError(GLFW_PLATFORM_UNAVAILABLE, "plan9: no draw(3) here (/dev/draw)");
        return GLFW_FALSE;
    }
    *platform = plan9;
    return GLFW_TRUE;
}

int _glfwInitPlan9(void)
{
    pthread_condattr_t attr;
    int key;
    uint32_t r;

    _glfw.plan9.mousefd = _glfw.plan9.kbdfd = _glfw.plan9.wctlfd = _glfw.plan9.cursorfd = -1;
    memset(_glfw.plan9.scancodes, -1, sizeof(_glfw.plan9.scancodes));
    // the runes of the keys (unshifted: lower case), the keys' scancodes
    for (r = 0x7f;  r > 0;  r--)
    {
        if ((r < 'A' || r > 'Z') && (key = _glfwKeyPlan9(r)) != GLFW_KEY_UNKNOWN)
            _glfw.plan9.scancodes[key] = (int) r;
    }
    for (r = Spec + 0xff;  r >= KF;  r--)
    {
        if ((key = _glfwKeyPlan9(r)) != GLFW_KEY_UNKNOWN)
            _glfw.plan9.scancodes[key] = (int) r;
    }

    pthread_mutex_init(&_glfw.plan9.lock, NULL);
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
    pthread_cond_init(&_glfw.plan9.cond, &attr);
    pthread_condattr_destroy(&attr);

    if (_glfwDrawInitPlan9() < 0)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "plan9: no draw(3) display");
        return GLFW_FALSE;
    }
    _glfwPollMonitorsPlan9();
    return GLFW_TRUE;
}

void _glfwTerminatePlan9(void)
{
    if (_glfw.plan9.cursorfd >= 0)
    {
        write(_glfw.plan9.cursorfd, "", 0);     // the window system's arrow again
        close(_glfw.plan9.cursorfd);
    }
    _glfw_free(_glfw.plan9.clipboard);
    _glfwTerminateEGL();
    _glfwTerminateOSMesa();
    _glfwDrawClosePlan9();
    // the readers block in their reads: they go with the process
}

const char* _glfwGetScancodeNamePlan9(int scancode)
{
    int key = _glfwKeyPlan9((uint32_t) scancode);

    // the printable keys: what they type
    if (scancode <= ' ' || scancode >= 0x7f || key == GLFW_KEY_UNKNOWN)
        return NULL;
    _glfw.plan9.keyname[0] = (char) scancode;
    _glfw.plan9.keyname[1] = 0;
    return _glfw.plan9.keyname;
}

int _glfwGetKeyScancodePlan9(int key)
{
    return _glfw.plan9.scancodes[key];
}

void _glfwSetClipboardStringPlan9(const char* string)
{
    int fd;

    if ((fd = open("/dev/snarf", O_WRONLY | O_TRUNC)) < 0)
    {
        _glfwInputError(GLFW_PLATFORM_ERROR, "plan9: no /dev/snarf");
        return;
    }
    write(fd, string, strlen(string));
    close(fd);
}

const char* _glfwGetClipboardStringPlan9(void)
{
    char* s = NULL;
    char* t;
    size_t n = 0;
    ssize_t m;
    int fd;

    if ((fd = open("/dev/snarf", O_RDONLY)) < 0)
    {
        _glfwInputError(GLFW_FORMAT_UNAVAILABLE, "plan9: no /dev/snarf");
        return NULL;
    }
    for (;;)
    {
        if (!(t = _glfw_realloc(s, n + 8192 + 1)))
            break;
        s = t;
        if ((m = read(fd, s + n, 8192)) <= 0)
            break;
        n += (size_t) m;
    }
    close(fd);
    if (!s)
        return NULL;
    s[n] = 0;
    _glfw_free(_glfw.plan9.clipboard);
    _glfw.plan9.clipboard = s;
    return s;
}

#endif // _GLFW_PLAN9
