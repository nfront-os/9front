/*
  SPDX-License-Identifier: Zlib

  SDL's plan9 video driver: OpenGL (ES) through EGL's plan9 platform.
*/
#ifndef SDL_plan9opengl_h_
#define SDL_plan9opengl_h_

#include "SDL_internal.h"
#include "video/SDL_sysvideo.h"

bool PLAN9_GL_LoadLibrary(SDL_VideoDevice *_this, const char *path);
SDL_FunctionPointer PLAN9_GL_GetProcAddress(SDL_VideoDevice *_this, const char *proc);
void PLAN9_GL_UnloadLibrary(SDL_VideoDevice *_this);
SDL_GLContext PLAN9_GL_CreateContext(SDL_VideoDevice *_this, SDL_Window *window);
bool PLAN9_GL_MakeCurrent(SDL_VideoDevice *_this, SDL_Window *window, SDL_GLContext context);
bool PLAN9_GL_SetSwapInterval(SDL_VideoDevice *_this, int interval);
bool PLAN9_GL_GetSwapInterval(SDL_VideoDevice *_this, int *interval);
bool PLAN9_GL_SwapWindow(SDL_VideoDevice *_this, SDL_Window *window);
bool PLAN9_GL_DestroyContext(SDL_VideoDevice *_this, SDL_GLContext context);

#endif
