//========================================================================
// GLFW 3.5 for 9front - www.glfw.org
// SPDX-License-Identifier: Zlib
//------------------------------------------------------------------------
// The plan9 platform's monitor: draw(3)'s screen, one, in the mode it is
// in (modes are set elsewhere: aux/vga, the GPU drivers' kms).  No gamma.
//========================================================================

#include "internal.h"

#if defined(_GLFW_PLAN9)

#include <string.h>

static GLFWvidmode mode(_GLFWmonitor* monitor)
{
    GLFWvidmode m;

    memset(&m, 0, sizeof(m));
    m.width = monitor->plan9.width;
    m.height = monitor->plan9.height;
    m.redBits = m.greenBits = m.blueBits = 8;
    m.refreshRate = 60;
    return m;
}

void _glfwPollMonitorsPlan9(void)
{
    _GLFWmonitor* monitor;
    int w, h;

    _glfwDrawScreenPlan9(&w, &h);
    // 96 dpi: no size of the screen is known
    monitor = _glfwAllocMonitor("screen", (int) (w * 25.4f / 96.f), (int) (h * 25.4f / 96.f));
    monitor->plan9.width = w;
    monitor->plan9.height = h;
    _glfwInputMonitor(monitor, GLFW_CONNECTED, _GLFW_INSERT_FIRST);
}

void _glfwFreeMonitorPlan9(_GLFWmonitor* monitor)
{
    (void) monitor;
}

void _glfwGetMonitorPosPlan9(_GLFWmonitor* monitor, int* xpos, int* ypos)
{
    (void) monitor;
    if (xpos)
        *xpos = 0;
    if (ypos)
        *ypos = 0;
}

void _glfwGetMonitorContentScalePlan9(_GLFWmonitor* monitor, float* xscale, float* yscale)
{
    (void) monitor;
    if (xscale)
        *xscale = 1.f;
    if (yscale)
        *yscale = 1.f;
}

void _glfwGetMonitorWorkareaPlan9(_GLFWmonitor* monitor,
                                  int* xpos, int* ypos,
                                  int* width, int* height)
{
    if (xpos)
        *xpos = 0;
    if (ypos)
        *ypos = 0;
    if (width)
        *width = monitor->plan9.width;
    if (height)
        *height = monitor->plan9.height;
}

GLFWvidmode* _glfwGetVideoModesPlan9(_GLFWmonitor* monitor, int* found)
{
    GLFWvidmode* modes = _glfw_calloc(1, sizeof(GLFWvidmode));

    *modes = mode(monitor);
    *found = 1;
    return modes;
}

GLFWbool _glfwGetVideoModePlan9(_GLFWmonitor* monitor, GLFWvidmode* m)
{
    *m = mode(monitor);
    return GLFW_TRUE;
}

GLFWbool _glfwGetGammaRampPlan9(_GLFWmonitor* monitor, GLFWgammaramp* ramp)
{
    (void) monitor;
    (void) ramp;
    _glfwInputError(GLFW_FEATURE_UNAVAILABLE, "plan9: no gamma ramps");
    return GLFW_FALSE;
}

void _glfwSetGammaRampPlan9(_GLFWmonitor* monitor, const GLFWgammaramp* ramp)
{
    (void) monitor;
    (void) ramp;
    _glfwInputError(GLFW_FEATURE_UNAVAILABLE, "plan9: no gamma ramps");
}

#endif // _GLFW_PLAN9
