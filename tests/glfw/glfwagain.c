/*
 * SPDX-License-Identifier: MIT
 *
 * glfwagain: a GLFW window (no client API: it draws nothing) that outlives
 * its wsys (tests/glfw/glfwagain.rc): events for secs seconds, the refresh
 * callback a line each ("event refresh": GLFW's plan9 platform asks for one
 * when the window is made again in the next wsys), whether it was closed.
 * Prints "ok ..." / "FAIL ..." lines and "glfwagain: all passed".
 */
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>

static int refreshes;

static void
refresh(GLFWwindow *w)
{
    (void) w;
    refreshes++;
    printf("event refresh\n");
}

int
main(int argc, char **argv)
{
    GLFWwindow *w;
    double t0;
    int secs;

    secs = argc > 1 ? atoi(argv[1]) : 20;
    setvbuf(stdout, NULL, _IOLBF, 0);
    if (!glfwInit()) {
        printf("FAIL glfwInit\n");
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    if ((w = glfwCreateWindow(300, 200, "glfwagain", NULL, NULL)) == NULL) {
        printf("FAIL glfwCreateWindow\n");
        return 1;
    }
    glfwSetWindowRefreshCallback(w, refresh);
    printf("ok window\n");
    t0 = glfwGetTime();
    while (!glfwWindowShouldClose(w) && glfwGetTime() - t0 < secs)
        glfwWaitEventsTimeout(0.2);
    printf("ok %d refreshes\n", refreshes);
    if (glfwWindowShouldClose(w))
        printf("FAIL it was closed\n");
    else
        printf("glfwagain: all passed\n");
    glfwDestroyWindow(w);
    glfwTerminate();
    return 0;
}
