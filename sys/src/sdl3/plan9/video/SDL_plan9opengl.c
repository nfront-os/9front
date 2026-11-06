/*
  SPDX-License-Identifier: Zlib

  SDL's plan9 video driver: OpenGL and OpenGL ES through EGL's plan9
  platform (Mesa's: zink on the GPU's Vulkan, the plan9 WSI), the native
  window "" being the process's window (/dev/winname) or the screen.
  9front has no dlopen: EGL is the static libvulkan.a (Mesa's), named here
  by weak references as Vulkan is (SDL_plan9vulkan.c); a program that
  wants SDL's GL links -lvulkan (and, if it calls no EGL function itself,
  -Wl,-u,eglGetDisplay).  SDL's own EGL layer (SDL_egl.c) loads libEGL by
  name and is not used.  One window a process, one surface.
*/
#include "SDL_internal.h"

#if defined(SDL_VIDEO_DRIVER_PLAN9) && (defined(SDL_VIDEO_OPENGL) || defined(SDL_VIDEO_OPENGL_ES2))

#include "SDL_plan9opengl.h"

#include <EGL/egl.h>

#define WEAK __attribute__((weak))
extern EGLDisplay EGLAPIENTRY eglGetDisplay(EGLNativeDisplayType) WEAK;
extern EGLBoolean EGLAPIENTRY eglInitialize(EGLDisplay, EGLint *, EGLint *) WEAK;
extern EGLBoolean EGLAPIENTRY eglTerminate(EGLDisplay) WEAK;
extern EGLBoolean EGLAPIENTRY eglChooseConfig(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *) WEAK;
extern EGLSurface EGLAPIENTRY eglCreateWindowSurface(EGLDisplay, EGLConfig, EGLNativeWindowType, const EGLint *) WEAK;
extern EGLBoolean EGLAPIENTRY eglDestroySurface(EGLDisplay, EGLSurface) WEAK;
extern EGLBoolean EGLAPIENTRY eglBindAPI(EGLenum) WEAK;
extern EGLContext EGLAPIENTRY eglCreateContext(EGLDisplay, EGLConfig, EGLContext, const EGLint *) WEAK;
extern EGLBoolean EGLAPIENTRY eglDestroyContext(EGLDisplay, EGLContext) WEAK;
extern EGLBoolean EGLAPIENTRY eglMakeCurrent(EGLDisplay, EGLSurface, EGLSurface, EGLContext) WEAK;
extern EGLBoolean EGLAPIENTRY eglSwapBuffers(EGLDisplay, EGLSurface) WEAK;
extern EGLBoolean EGLAPIENTRY eglSwapInterval(EGLDisplay, EGLint) WEAK;
extern EGLint EGLAPIENTRY eglGetError(void) WEAK;
extern __eglMustCastToProperFunctionPointerType EGLAPIENTRY eglGetProcAddress(const char *) WEAK;

#ifndef EGL_CONTEXT_MAJOR_VERSION
#define EGL_CONTEXT_MAJOR_VERSION 0x3098
#define EGL_CONTEXT_MINOR_VERSION 0x30FB
#define EGL_CONTEXT_OPENGL_PROFILE_MASK 0x30FD
#define EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT 0x00000001
#define EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT 0x00000002
#define EGL_OPENGL_ES3_BIT 0x00000040
#endif

static EGLDisplay dpy = EGL_NO_DISPLAY;
static EGLSurface surf = EGL_NO_SURFACE;	/* the window's: one a process */
static EGLConfig cfg;
static int swapinterval = 1;

static bool eglerror(const char *what)
{
    return SDL_SetError("plan9: %s: EGL error %#x", what, eglGetError ? eglGetError() : 0);
}

bool PLAN9_GL_LoadLibrary(SDL_VideoDevice *_this, const char *path)
{
    EGLint maj, min;

    (void)path;
    if (!eglGetDisplay || !eglGetProcAddress) {
        return SDL_SetError("plan9: no EGL in the program (link -lvulkan)");
    }
    if (dpy == EGL_NO_DISPLAY) {
        dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, &maj, &min)) {
            dpy = EGL_NO_DISPLAY;
            return eglerror("eglInitialize");
        }
    }
    SDL_strlcpy(_this->gl_config.driver_path, "libvulkan.a", SDL_arraysize(_this->gl_config.driver_path));
    return true;
}

SDL_FunctionPointer PLAN9_GL_GetProcAddress(SDL_VideoDevice *_this, const char *proc)
{
    (void)_this;
    /* Mesa's EGL gives core functions too (EGL_KHR_get_all_proc_addresses) */
    return eglGetProcAddress ? (SDL_FunctionPointer)eglGetProcAddress(proc) : NULL;
}

void PLAN9_GL_UnloadLibrary(SDL_VideoDevice *_this)
{
    (void)_this;
    if (dpy != EGL_NO_DISPLAY) {
        if (surf != EGL_NO_SURFACE) {
            eglDestroySurface(dpy, surf);
            surf = EGL_NO_SURFACE;
        }
        eglTerminate(dpy);
        dpy = EGL_NO_DISPLAY;
    }
}

/* a config for SDL's attributes: GL or ES, the sizes asked for */
static bool chooseconfig(SDL_VideoDevice *_this, bool es)
{
    EGLint attr[32], n;
    int i = 0;

    attr[i++] = EGL_SURFACE_TYPE;
    attr[i++] = EGL_WINDOW_BIT;
    attr[i++] = EGL_RENDERABLE_TYPE;
    attr[i++] = es ? (_this->gl_config.major_version >= 3 ? EGL_OPENGL_ES3_BIT : EGL_OPENGL_ES2_BIT) : EGL_OPENGL_BIT;
    attr[i++] = EGL_RED_SIZE;
    attr[i++] = _this->gl_config.red_size;
    attr[i++] = EGL_GREEN_SIZE;
    attr[i++] = _this->gl_config.green_size;
    attr[i++] = EGL_BLUE_SIZE;
    attr[i++] = _this->gl_config.blue_size;
    if (_this->gl_config.alpha_size) {
        attr[i++] = EGL_ALPHA_SIZE;
        attr[i++] = _this->gl_config.alpha_size;
    }
    if (_this->gl_config.depth_size) {
        attr[i++] = EGL_DEPTH_SIZE;
        attr[i++] = _this->gl_config.depth_size;
    }
    if (_this->gl_config.stencil_size) {
        attr[i++] = EGL_STENCIL_SIZE;
        attr[i++] = _this->gl_config.stencil_size;
    }
    attr[i++] = EGL_NONE;
    if (!eglChooseConfig(dpy, attr, &cfg, 1, &n) || n < 1) {
        return eglerror("eglChooseConfig");
    }
    return true;
}

SDL_GLContext PLAN9_GL_CreateContext(SDL_VideoDevice *_this, SDL_Window *window)
{
    EGLint attr[16];
    EGLContext ctx, share;
    bool es;
    int i = 0;

    (void)window;
    if (dpy == EGL_NO_DISPLAY && !PLAN9_GL_LoadLibrary(_this, NULL)) {
        return NULL;
    }
    es = _this->gl_config.profile_mask == SDL_GL_CONTEXT_PROFILE_ES;
    if (surf == EGL_NO_SURFACE) {
        if (!chooseconfig(_this, es)) {
            return NULL;
        }
        surf = eglCreateWindowSurface(dpy, cfg, (EGLNativeWindowType)"", NULL);	/* "": this process's window */
        if (surf == EGL_NO_SURFACE) {
            eglerror("eglCreateWindowSurface");
            return NULL;
        }
    }
    if (!eglBindAPI(es ? EGL_OPENGL_ES_API : EGL_OPENGL_API)) {
        eglerror("eglBindAPI");
        return NULL;
    }
    attr[i++] = EGL_CONTEXT_MAJOR_VERSION;
    attr[i++] = _this->gl_config.major_version;
    attr[i++] = EGL_CONTEXT_MINOR_VERSION;
    attr[i++] = _this->gl_config.minor_version;
    if (!es && _this->gl_config.profile_mask) {
        attr[i++] = EGL_CONTEXT_OPENGL_PROFILE_MASK;
        attr[i++] = _this->gl_config.profile_mask == SDL_GL_CONTEXT_PROFILE_CORE ?
            EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT : EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT;
    }
    attr[i++] = EGL_NONE;
    share = _this->gl_config.share_with_current_context ? (EGLContext)SDL_GL_GetCurrentContext() : EGL_NO_CONTEXT;
    ctx = eglCreateContext(dpy, cfg, share, attr);
    if (ctx == EGL_NO_CONTEXT) {
        eglerror("eglCreateContext");
        return NULL;
    }
    if (!eglMakeCurrent(dpy, surf, surf, ctx)) {
        eglDestroyContext(dpy, ctx);
        eglerror("eglMakeCurrent");
        return NULL;
    }
    eglSwapInterval(dpy, swapinterval);
    return (SDL_GLContext)ctx;
}

bool PLAN9_GL_MakeCurrent(SDL_VideoDevice *_this, SDL_Window *window, SDL_GLContext context)
{
    (void)_this;
    if (dpy == EGL_NO_DISPLAY) {
        return SDL_SetError("plan9: EGL not loaded");
    }
    if (!context || !window) {
        return eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT) ? true : eglerror("eglMakeCurrent");
    }
    return eglMakeCurrent(dpy, surf, surf, (EGLContext)context) ? true : eglerror("eglMakeCurrent");
}

bool PLAN9_GL_SetSwapInterval(SDL_VideoDevice *_this, int interval)
{
    (void)_this;
    if (interval < 0) {
        interval = 1;	/* no late swaps: vsync */
    }
    if (dpy != EGL_NO_DISPLAY && !eglSwapInterval(dpy, interval)) {
        return eglerror("eglSwapInterval");
    }
    swapinterval = interval;
    return true;
}

bool PLAN9_GL_GetSwapInterval(SDL_VideoDevice *_this, int *interval)
{
    (void)_this;
    *interval = swapinterval;
    return true;
}

bool PLAN9_GL_SwapWindow(SDL_VideoDevice *_this, SDL_Window *window)
{
    (void)_this;
    (void)window;
    return eglSwapBuffers(dpy, surf) ? true : eglerror("eglSwapBuffers");
}

bool PLAN9_GL_DestroyContext(SDL_VideoDevice *_this, SDL_GLContext context)
{
    (void)_this;
    if (dpy != EGL_NO_DISPLAY && context) {
        eglDestroyContext(dpy, (EGLContext)context);
    }
    return true;
}

#endif
