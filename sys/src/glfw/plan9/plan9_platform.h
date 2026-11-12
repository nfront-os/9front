//========================================================================
// GLFW 3.5 for 9front - www.glfw.org
// SPDX-License-Identifier: Zlib
//------------------------------------------------------------------------
// The plan9 platform: the process's window of wsys or rio (the screen
// without either), as draw(3) names it (plan9_draw.c); input from
// /dev/mouse and /dev/kbd, read by threads of their own into a queue
// that the event functions empty; OpenGL and GLES through EGL's plan9
// platform, Vulkan through VK_PLAN9_surface, both found by weak
// references in the static libvulkan.a (plan9_module.c: no dlopen).
// One window a process.
//========================================================================

#include <pthread.h>
#include <stdint.h>
#include <time.h>

#define GLFW_PLAN9_WINDOW_STATE          _GLFWwindowPlan9 plan9;
#define GLFW_PLAN9_LIBRARY_WINDOW_STATE  _GLFWlibraryPlan9 plan9;
#define GLFW_PLAN9_MONITOR_STATE         _GLFWmonitorPlan9 plan9;
#define GLFW_PLAN9_CURSOR_STATE          _GLFWcursorPlan9 plan9;

// VK_PLAN9_surface (9front's vulkan/vulkan_plan9.h)
#define VK_STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9 ((VkStructureType)1008999000)

typedef struct VkPlan9SurfaceCreateInfoPLAN9
{
    VkStructureType sType;
    const void*     pNext;
    VkFlags         flags;
    const char*     pWindowName;    // NULL: /dev/winname, else the screen
} VkPlan9SurfaceCreateInfoPLAN9;

typedef VkResult (APIENTRY *PFN_vkCreatePlan9SurfacePLAN9)(VkInstance,const VkPlan9SurfaceCreateInfoPLAN9*,const VkAllocationCallbacks*,VkSurfaceKHR*);

// what the readers read, for the event functions
typedef struct _GLFWeventPlan9
{
    char            kind;       // m mouse, r resized, k K c keyboard, w wctl, q the end
    int             x, y, buttons;
    char            text[128];  // k K c: UTF-8; w: wctl's line
} _GLFWeventPlan9;

#define _GLFW_PLAN9_EVENTS  512
#define _GLFW_PLAN9_KEYS    32

typedef struct _GLFWwindowPlan9
{
    int             xpos, ypos;         // the contents on the screen
    int             width, height;
    GLFWbool        visible;
    GLFWbool        iconified;
    GLFWbool        maximized;
    int             restore[4];         // before maximizing: x y w h
    GLFWbool        hovered;
    double          lastx, lasty;       // the pointer in the window
    double          virtx, virty;       // GLFW_CURSOR_DISABLED: where it would be
} _GLFWwindowPlan9;

typedef struct _GLFWmonitorPlan9
{
    int             width, height;
} _GLFWmonitorPlan9;

typedef struct _GLFWcursorPlan9
{
    unsigned char   c[72];      // mouse(3)'s cursor: offset, clr, set
    GLFWbool        arrow;      // the window system's own
} _GLFWcursorPlan9;

typedef struct _GLFWlibraryPlan9
{
    _GLFWwindow*    window;     // the one
    int             mousefd, kbdfd, wctlfd, cursorfd;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
    _GLFWeventPlan9 events[_GLFW_PLAN9_EVENTS];
    int             head, tail;
    GLFWbool        wake;       // glfwPostEmptyEvent
    GLFWbool        readers;
    int             buttons;
    uint32_t        keys[_GLFW_PLAN9_KEYS];     // down, as the last k/K said
    int             nkeys;
    int             border;     // a window system's border around the contents
    int             dpi;        // its dots per inch (wsys' scale ×96): the content scale
    char*           clipboard;
    int             scancodes[GLFW_KEY_LAST + 1];   // a key's rune
    char            keyname[8];
    pthread_t       mousereader;    // keyboard's and wctl's: detached (startreaders)
    GLFWbool        focused, hidden;
} _GLFWlibraryPlan9;

GLFWbool _glfwConnectPlan9(int platformID, _GLFWplatform* platform);
int _glfwInitPlan9(void);
void _glfwTerminatePlan9(void);
int _glfwKeyPlan9(uint32_t rune);
int _glfwModsPlan9(void);
void _glfwShowCursorPlan9(_GLFWwindow* window);

void _glfwPollMonitorsPlan9(void);
void _glfwFreeMonitorPlan9(_GLFWmonitor* monitor);
void _glfwGetMonitorPosPlan9(_GLFWmonitor* monitor, int* xpos, int* ypos);
void _glfwGetMonitorContentScalePlan9(_GLFWmonitor* monitor, float* xscale, float* yscale);
void _glfwGetMonitorWorkareaPlan9(_GLFWmonitor* monitor, int* xpos, int* ypos, int* width, int* height);
GLFWvidmode* _glfwGetVideoModesPlan9(_GLFWmonitor* monitor, int* found);
GLFWbool _glfwGetVideoModePlan9(_GLFWmonitor* monitor, GLFWvidmode* mode);
GLFWbool _glfwGetGammaRampPlan9(_GLFWmonitor* monitor, GLFWgammaramp* ramp);
void _glfwSetGammaRampPlan9(_GLFWmonitor* monitor, const GLFWgammaramp* ramp);

GLFWbool _glfwCreateWindowPlan9(_GLFWwindow* window, const _GLFWwndconfig* wndconfig, const _GLFWctxconfig* ctxconfig, const _GLFWfbconfig* fbconfig);
void _glfwDestroyWindowPlan9(_GLFWwindow* window);
void _glfwSetWindowTitlePlan9(_GLFWwindow* window, const char* title);
void _glfwSetWindowIconPlan9(_GLFWwindow* window, int count, const GLFWimage* images);
void _glfwSetWindowMonitorPlan9(_GLFWwindow* window, _GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate);
void _glfwGetWindowPosPlan9(_GLFWwindow* window, int* xpos, int* ypos);
void _glfwSetWindowPosPlan9(_GLFWwindow* window, int xpos, int ypos);
void _glfwGetWindowSizePlan9(_GLFWwindow* window, int* width, int* height);
void _glfwSetWindowSizePlan9(_GLFWwindow* window, int width, int height);
void _glfwSetWindowSizeLimitsPlan9(_GLFWwindow* window, int minwidth, int minheight, int maxwidth, int maxheight);
void _glfwSetWindowAspectRatioPlan9(_GLFWwindow* window, int n, int d);
void _glfwGetFramebufferSizePlan9(_GLFWwindow* window, int* width, int* height);
void _glfwGetWindowFrameSizePlan9(_GLFWwindow* window, int* left, int* top, int* right, int* bottom);
void _glfwGetWindowContentScalePlan9(_GLFWwindow* window, float* xscale, float* yscale);
void _glfwIconifyWindowPlan9(_GLFWwindow* window);
void _glfwRestoreWindowPlan9(_GLFWwindow* window);
void _glfwMaximizeWindowPlan9(_GLFWwindow* window);
GLFWbool _glfwWindowMaximizedPlan9(_GLFWwindow* window);
GLFWbool _glfwWindowHoveredPlan9(_GLFWwindow* window);
GLFWbool _glfwFramebufferTransparentPlan9(_GLFWwindow* window);
void _glfwSetWindowResizablePlan9(_GLFWwindow* window, GLFWbool enabled);
void _glfwSetWindowDecoratedPlan9(_GLFWwindow* window, GLFWbool enabled);
void _glfwSetWindowFloatingPlan9(_GLFWwindow* window, GLFWbool enabled);
void _glfwSetWindowMousePassthroughPlan9(_GLFWwindow* window, GLFWbool enabled);
float _glfwGetWindowOpacityPlan9(_GLFWwindow* window);
void _glfwSetWindowOpacityPlan9(_GLFWwindow* window, float opacity);
void _glfwSetRawMouseMotionPlan9(_GLFWwindow *window, GLFWbool enabled);
GLFWbool _glfwRawMouseMotionSupportedPlan9(void);
void _glfwShowWindowPlan9(_GLFWwindow* window);
void _glfwRequestWindowAttentionPlan9(_GLFWwindow* window);
void _glfwHideWindowPlan9(_GLFWwindow* window);
void _glfwFocusWindowPlan9(_GLFWwindow* window);
GLFWbool _glfwWindowFocusedPlan9(_GLFWwindow* window);
GLFWbool _glfwWindowIconifiedPlan9(_GLFWwindow* window);
GLFWbool _glfwWindowVisiblePlan9(_GLFWwindow* window);
void _glfwPollEventsPlan9(void);
void _glfwWaitEventsPlan9(void);
void _glfwWaitEventsTimeoutPlan9(double timeout);
void _glfwPostEmptyEventPlan9(void);
void _glfwGetCursorPosPlan9(_GLFWwindow* window, double* xpos, double* ypos);
void _glfwSetCursorPosPlan9(_GLFWwindow* window, double x, double y);
void _glfwSetCursorModePlan9(_GLFWwindow* window, int mode);
GLFWbool _glfwCreateCursorPlan9(_GLFWcursor* cursor, const GLFWimage* image, int xhot, int yhot);
GLFWbool _glfwCreateStandardCursorPlan9(_GLFWcursor* cursor, int shape);
void _glfwDestroyCursorPlan9(_GLFWcursor* cursor);
void _glfwSetCursorPlan9(_GLFWwindow* window, _GLFWcursor* cursor);
void _glfwSetClipboardStringPlan9(const char* string);
const char* _glfwGetClipboardStringPlan9(void);
const char* _glfwGetScancodeNamePlan9(int scancode);
int _glfwGetKeyScancodePlan9(int key);

EGLenum _glfwGetEGLPlatformPlan9(EGLint** attribs);
EGLNativeDisplayType _glfwGetEGLNativeDisplayPlan9(void);
EGLNativeWindowType _glfwGetEGLNativeWindowPlan9(_GLFWwindow* window);

void _glfwGetRequiredInstanceExtensionsPlan9(char** extensions);
GLFWbool _glfwGetPhysicalDevicePresentationSupportPlan9(VkInstance instance, VkPhysicalDevice device, uint32_t queuefamily);
VkResult _glfwCreateWindowSurfacePlan9(VkInstance instance, _GLFWwindow* window, const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface);

#include "plan9_draw.h"
