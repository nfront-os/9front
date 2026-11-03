/*
 * SPDX-License-Identifier: MIT
 *
 * gltest: OpenGL ES 2 through Mesa's zink (GL on the Vulkan drivers) and
 * EGL (its native platform, plan9), without a window: a 64 x 64 pbuffer
 * cleared red, a green triangle drawn over its middle, read back with
 * glReadPixels and checked.  The renderer's name says which Vulkan
 * device zink took.
 *	gltest
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

enum { W = 64, H = 64 };

static int failed;

static void
check(int ok, char *what)
{
	printf("%s %s\n", ok ? "ok" : "FAIL", what);
	if(!ok)
		failed++;
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

/* the pixel at x y, RGBA */
static unsigned
pixel(int x, int y)
{
	unsigned char p[4];

	glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, p);
	return p[0]<<24 | p[1]<<16 | p[2]<<8 | p[3];
}

int
main(void)
{
	static const EGLint cattr[] = {
		EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
		EGL_NONE,
	};
	static const EGLint sattr[] = { EGL_WIDTH, W, EGL_HEIGHT, H, EGL_NONE };
	static const EGLint xattr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	static const GLfloat tri[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };
	EGLDisplay d;
	EGLConfig c;
	EGLSurface s;
	EGLContext x;
	EGLint maj, min, n;
	GLuint p;
	unsigned v;

	d = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(d == EGL_NO_DISPLAY || !eglInitialize(d, &maj, &min)){
		printf("FAIL eglInitialize: %#x\n", eglGetError());
		return 1;
	}
	printf("EGL %d.%d %s\n", maj, min, eglQueryString(d, EGL_VENDOR));
	if(!eglChooseConfig(d, cattr, &c, 1, &n) || n < 1){
		printf("FAIL no config: %#x\n", eglGetError());
		return 1;
	}
	s = eglCreatePbufferSurface(d, c, sattr);
	eglBindAPI(EGL_OPENGL_ES_API);
	x = eglCreateContext(d, c, EGL_NO_CONTEXT, xattr);
	if(s == EGL_NO_SURFACE || x == EGL_NO_CONTEXT || !eglMakeCurrent(d, s, s, x)){
		printf("FAIL context: %#x\n", eglGetError());
		return 1;
	}
	printf("renderer %s\nversion %s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

	glViewport(0, 0, W, H);
	glClearColor(1, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	v = pixel(2, 2);
	printf("cleared: %08x\n", v);
	check(v == 0xff0000ff, "clear red");

	p = glCreateProgram();
	glAttachShader(p, shader(GL_VERTEX_SHADER,
		"attribute vec2 pos;\n"
		"void main() { gl_Position = vec4(pos, 0.0, 1.0); }\n"));
	glAttachShader(p, shader(GL_FRAGMENT_SHADER,
		"precision mediump float;\n"
		"void main() { gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0); }\n"));
	glBindAttribLocation(p, 0, "pos");
	glLinkProgram(p);
	glUseProgram(p);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, tri);
	glEnableVertexAttribArray(0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glFinish();
	v = pixel(W/2, H/2);
	printf("middle: %08x\n", v);
	check(v == 0x00ff00ff, "triangle green in the middle");
	v = pixel(2, H-3);
	check(v == 0xff0000ff, "corner still red");
	check(glGetError() == GL_NO_ERROR, "no GL error");

	eglMakeCurrent(d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglTerminate(d);
	printf("gltest: %s\n", failed ? "FAILED" : "all passed");
	return failed != 0;
}
