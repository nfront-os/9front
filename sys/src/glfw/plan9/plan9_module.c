//========================================================================
// GLFW 3.5 for 9front - www.glfw.org
// SPDX-License-Identifier: Zlib
//------------------------------------------------------------------------
// GLFW loads EGL, the GL client library and the Vulkan loader by name
// (dlopen).  9front has no dlopen: EGL, GL and Vulkan are the static
// libvulkan.a (Mesa's).  Here the names are answered from what the
// program has linked, by weak references, so that a program without GL
// or Vulkan does not carry them; one that wants them links -lvulkan (and
// -Wl,-u,eglGetDisplay or -Wl,-u,vkGetInstanceProcAddr if it calls no
// such function itself).  Without internal.h: its macros name the EGL
// functions after GLFW's pointers to them.
//========================================================================

#include <GLFW/glfw3.h>

#include <string.h>

typedef void (*GLFWproc)(void);     // as internal.h has it

void* _glfwPlatformLoadModule(const char* path);
void _glfwPlatformFreeModule(void* module);
GLFWproc _glfwPlatformGetModuleSymbol(void* module, const char* name);

#define WEAK __attribute__((weak))

// declared as what they are to the linker only: their addresses are taken
extern void eglGetConfigAttrib(void) WEAK;
extern void eglGetConfigs(void) WEAK;
extern void eglGetDisplay(void) WEAK;
extern void eglGetError(void) WEAK;
extern void eglInitialize(void) WEAK;
extern void eglTerminate(void) WEAK;
extern void eglBindAPI(void) WEAK;
extern void eglCreateContext(void) WEAK;
extern void eglDestroySurface(void) WEAK;
extern void eglDestroyContext(void) WEAK;
extern void eglCreateWindowSurface(void) WEAK;
extern void eglCreatePbufferSurface(void) WEAK;
extern void eglMakeCurrent(void) WEAK;
extern void eglSwapBuffers(void) WEAK;
extern void eglSwapInterval(void) WEAK;
extern void eglQueryString(void) WEAK;
extern GLFWproc eglGetProcAddress(const char*) WEAK;
extern GLFWproc vkGetInstanceProcAddr(void*, const char*) WEAK;

static const struct
{
    const char* name;
    void (*f)(void);
} egl[] =
{
    { "eglGetConfigAttrib", eglGetConfigAttrib },
    { "eglGetConfigs", eglGetConfigs },
    { "eglGetDisplay", eglGetDisplay },
    { "eglGetError", eglGetError },
    { "eglInitialize", eglInitialize },
    { "eglTerminate", eglTerminate },
    { "eglBindAPI", eglBindAPI },
    { "eglCreateContext", eglCreateContext },
    { "eglDestroySurface", eglDestroySurface },
    { "eglDestroyContext", eglDestroyContext },
    { "eglCreateWindowSurface", eglCreateWindowSurface },
    { "eglCreatePbufferSurface", eglCreatePbufferSurface },
    { "eglMakeCurrent", eglMakeCurrent },
    { "eglSwapBuffers", eglSwapBuffers },
    { "eglSwapInterval", eglSwapInterval },
    { "eglQueryString", eglQueryString },
    { "eglGetProcAddress", (void (*)(void)) eglGetProcAddress },
};

// the libraries GLFW asks for: EGL, GL or GLES (Mesa's EGL gives their functions), Vulkan
static char libegl, libgl, libvulkan;

void* _glfwPlatformLoadModule(const char* path)
{
    const char* base = strrchr(path, '/');

    base = base ? base + 1 : path;
    if (strncmp(base, "libEGL", 6) == 0)
        return eglGetDisplay && eglGetProcAddress ? &libegl : NULL;
    if (strncmp(base, "libGL", 5) == 0)
        return eglGetProcAddress ? &libgl : NULL;
    if (strncmp(base, "libvulkan", 9) == 0)
        return vkGetInstanceProcAddr ? &libvulkan : NULL;
    return NULL;    // OSMesa and the rest: not here
}

void _glfwPlatformFreeModule(void* module)
{
    (void) module;
}

GLFWproc _glfwPlatformGetModuleSymbol(void* module, const char* name)
{
    size_t i;

    if (module == &libegl)
    {
        for (i = 0;  i < sizeof(egl) / sizeof(egl[0]);  i++)
        {
            if (strcmp(egl[i].name, name) == 0)
                return egl[i].f;
        }
        return eglGetProcAddress(name);
    }
    if (module == &libgl)
        return eglGetProcAddress(name);
    if (module == &libvulkan)
    {
        if (strcmp(name, "vkGetInstanceProcAddr") == 0)
            return (GLFWproc) vkGetInstanceProcAddr;
        return vkGetInstanceProcAddr(NULL, name);
    }
    return NULL;
}
