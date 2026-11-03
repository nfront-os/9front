/*
 * SPDX-License-Identifier: MIT
 *
 * gldesk: desktop OpenGL through zink and EGL (eglBindAPI(EGL_OPENGL_API)),
 * without a window (surfaceless, a pbuffer): a compatibility context
 * draws a triangle the old way (glBegin/glEnd, fixed function), a core
 * one (the newest version zink gives, 4.x) with a vertex array object,
 * a buffer and GLSL 3.30; each read back (glReadPixels) and checked.
 *	gldesk
 */
#include <stdio.h>
#include <string.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/glext.h>

enum { W = 64, H = 64 };

static int failed;

static void
check(int ok, char *what)
{
	printf("%s %s\n", ok ? "ok" : "FAIL", what);
	if(!ok)
		failed++;
}

static unsigned
pixel(int x, int y)
{
	unsigned char p[4];

	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, p);
	return p[0]<<24 | p[1]<<16 | p[2]<<8 | p[3];
}

static GLuint
shader(GLenum type, const char *src)
{
	GLuint s;
	GLint ok;
	char log[512];

	s = glCreateShader(type);
	glShaderSource(s, 1, &src, NULL);
	glCompileShader(s);
	glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
	if(!ok){
		glGetShaderInfoLog(s, sizeof log, NULL, log);
		printf("FAIL shader: %s\n", log);
		failed++;
	}
	return s;
}

/* a context of the profile, current on a new pbuffer; nil if none */
static EGLContext
context(EGLDisplay d, EGLConfig c, EGLint profile, EGLint major, EGLint minor, EGLSurface *s)
{
	static const EGLint sattr[] = { EGL_WIDTH, W, EGL_HEIGHT, H, EGL_NONE };
	EGLint xattr[] = {
		EGL_CONTEXT_MAJOR_VERSION, major,
		EGL_CONTEXT_MINOR_VERSION, minor,
		EGL_CONTEXT_OPENGL_PROFILE_MASK, profile,
		EGL_NONE,
	};
	EGLContext x;

	*s = eglCreatePbufferSurface(d, c, sattr);
	x = eglCreateContext(d, c, EGL_NO_CONTEXT, xattr);
	if(*s == EGL_NO_SURFACE || x == EGL_NO_CONTEXT || !eglMakeCurrent(d, *s, *s, x))
		return EGL_NO_CONTEXT;
	return x;
}

static void
compat(EGLDisplay d, EGLConfig c)
{
	EGLSurface s;
	unsigned v;

	if(context(d, c, EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT, 2, 1, &s) == EGL_NO_CONTEXT){
		printf("FAIL compatibility context: %#x\n", eglGetError());
		failed++;
		return;
	}
	printf("compat: renderer %s\ncompat: version %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));
	glViewport(0, 0, W, H);
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 0);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.5f, -0.5f);
	glVertex2f(0.0f, 0.5f);
	glEnd();
	glFinish();
	v = pixel(W/2, H/2);
	printf("compat: middle %08x\n", v);
	check(v == 0xffff00ff, "compat: glBegin triangle yellow in the middle");
	check(pixel(2, H-3) == 0x0000ffff, "compat: corner blue");
	check(glGetError() == GL_NO_ERROR, "compat: no GL error");
	eglMakeCurrent(d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

static void
core(EGLDisplay d, EGLConfig c)
{
	static const GLfloat tri[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };
	EGLSurface s;
	GLuint vao, vbo, p;
	GLint maj, min;
	unsigned v;

	if(context(d, c, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, 4, 5, &s) == EGL_NO_CONTEXT &&
	   context(d, c, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT, 3, 3, &s) == EGL_NO_CONTEXT){
		printf("FAIL core context: %#x\n", eglGetError());
		failed++;
		return;
	}
	glGetIntegerv(GL_MAJOR_VERSION, &maj);
	glGetIntegerv(GL_MINOR_VERSION, &min);
	printf("core: version %s (%d.%d), GLSL %s\n", glGetString(GL_VERSION), maj, min,
		glGetString(GL_SHADING_LANGUAGE_VERSION));
	check(maj >= 4, "core: OpenGL 4 or later");
	glViewport(0, 0, W, H);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof tri, tri, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	p = glCreateProgram();
	glAttachShader(p, shader(GL_VERTEX_SHADER,
		"#version 330 core\n"
		"layout(location = 0) in vec2 pos;\n"
		"void main() { gl_Position = vec4(pos, 0.0, 1.0); }\n"));
	glAttachShader(p, shader(GL_FRAGMENT_SHADER,
		"#version 330 core\n"
		"out vec4 color;\n"
		"void main() { color = vec4(0.0, 1.0, 1.0, 1.0); }\n"));
	glLinkProgram(p);
	glUseProgram(p);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glFinish();
	v = pixel(W/2, H/2);
	printf("core: middle %08x\n", v);
	check(v == 0x00ffffff, "core: GLSL 3.30 triangle cyan in the middle");
	check(pixel(2, H-3) == 0x000000ff, "core: corner black");
	check(glGetError() == GL_NO_ERROR, "core: no GL error");
	eglMakeCurrent(d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
}

int
main(void)
{
	static const EGLint cattr[] = {
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
		EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
		EGL_NONE,
	};
	EGLDisplay d;
	EGLConfig c;
	EGLint maj, min, n;

	d = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(d == EGL_NO_DISPLAY || !eglInitialize(d, &maj, &min)){
		printf("FAIL eglInitialize: %#x\n", eglGetError());
		return 1;
	}
	printf("EGL %d.%d, client APIs %s\n", maj, min, eglQueryString(d, EGL_CLIENT_APIS));
	if(!eglBindAPI(EGL_OPENGL_API)){
		printf("FAIL eglBindAPI(EGL_OPENGL_API): %#x\n", eglGetError());
		return 1;
	}
	if(!eglChooseConfig(d, cattr, &c, 1, &n) || n < 1){
		printf("FAIL no desktop GL config: %#x\n", eglGetError());
		return 1;
	}
	compat(d, c);
	core(d, c);
	eglTerminate(d);
	printf("gldesk: %s\n", failed ? "FAILED" : "all passed");
	return failed != 0;
}
