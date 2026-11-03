/*
 * SPDX-License-Identifier: MIT
 *
 * glwin: OpenGL ES 2 in a draw(3) window, through EGL's plan9 platform
 * (zink, kopper, Mesa's plan9 WSI): the program's window (or the screen
 * without rio), a triangle turning over a blue background for n frames
 * (eglSwapBuffers each); each frame's middle is checked before the swap
 * (glReadPixels), the frames per second at the end.
 *	glwin [frames]
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

static double
now(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec + t.tv_nsec / 1e9;
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
		exit(1);
	}
	return s;
}

int
main(int argc, char **argv)
{
	static const EGLint cattr[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8,
		EGL_NONE,
	};
	static const EGLint xattr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	static const GLfloat tri[] = { 0.0f, 0.6f, -0.52f, -0.3f, 0.52f, -0.3f };
	EGLDisplay d;
	EGLConfig c;
	EGLSurface s;
	EGLContext x;
	EGLint maj, min, n, w, h;
	GLuint p;
	GLint rot;
	unsigned char px[4];
	double t0, t;
	int frames, i, bad;

	frames = argc > 1 ? atoi(argv[1]) : 300;
	d = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	if(d == EGL_NO_DISPLAY || !eglInitialize(d, &maj, &min)){
		printf("FAIL eglInitialize: %#x\n", eglGetError());
		return 1;
	}
	if(!eglChooseConfig(d, cattr, &c, 1, &n) || n < 1){
		printf("FAIL no window config: %#x\n", eglGetError());
		return 1;
	}
	s = eglCreateWindowSurface(d, c, (EGLNativeWindowType)"", NULL);	/* "": this program's window */
	if(s == EGL_NO_SURFACE){
		printf("FAIL eglCreateWindowSurface: %#x\n", eglGetError());
		return 1;
	}
	eglBindAPI(EGL_OPENGL_ES_API);
	x = eglCreateContext(d, c, EGL_NO_CONTEXT, xattr);
	if(x == EGL_NO_CONTEXT || !eglMakeCurrent(d, s, s, x)){
		printf("FAIL context: %#x\n", eglGetError());
		return 1;
	}
	eglQuerySurface(d, s, EGL_WIDTH, &w);
	eglQuerySurface(d, s, EGL_HEIGHT, &h);
	printf("EGL %d.%d %s, window %dx%d\nrenderer %s\n", maj, min,
		eglQueryString(d, EGL_VENDOR), w, h, glGetString(GL_RENDERER));

	p = glCreateProgram();
	glAttachShader(p, shader(GL_VERTEX_SHADER,
		"attribute vec2 pos;\n"
		"uniform float rot;\n"
		"void main() {\n"
		"	float c = cos(rot), s = sin(rot);\n"
		"	gl_Position = vec4(c*pos.x - s*pos.y, s*pos.x + c*pos.y, 0.0, 1.0);\n"
		"}\n"));
	glAttachShader(p, shader(GL_FRAGMENT_SHADER,
		"precision mediump float;\n"
		"void main() { gl_FragColor = vec4(1.0, 0.8, 0.0, 1.0); }\n"));
	glBindAttribLocation(p, 0, "pos");
	glLinkProgram(p);
	glUseProgram(p);
	rot = glGetUniformLocation(p, "rot");
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, tri);
	glEnableVertexAttribArray(0);

	bad = 0;
	t0 = now();
	for(i = 0; i < frames; i++){
		eglQuerySurface(d, s, EGL_WIDTH, &w);
		eglQuerySurface(d, s, EGL_HEIGHT, &h);
		glViewport(0, 0, w, h);
		glClearColor(0.1f, 0.2f, 0.5f, 1);
		glClear(GL_COLOR_BUFFER_BIT);
		glUniform1f(rot, i * 0.03f);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		/* the middle is the triangle's, yellow */
		glReadPixels(w/2, h/2, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, px);
		if(px[0] < 0xf0 || px[1] < 0xc0 || px[2] > 0x10)
			bad++;
		if(!eglSwapBuffers(d, s)){
			printf("FAIL eglSwapBuffers frame %d: %#x\n", i, eglGetError());
			return 1;
		}
	}
	t = now() - t0;
	printf("glwin: %d frames %dx%d in %.2f s: %.1f fps, %d frames wrong in the middle\n",
		frames, w, h, t, frames / t, bad);
	printf("%s\n", bad == 0 && glGetError() == GL_NO_ERROR ? "glwin: all passed" : "glwin: FAILED");
	eglMakeCurrent(d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroySurface(d, s);
	eglTerminate(d);
	return bad != 0;
}
