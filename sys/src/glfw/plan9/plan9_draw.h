/*
 * GLFW 3.5 for 9front - www.glfw.org
 * SPDX-License-Identifier: Zlib
 *
 * plan9_draw.c, the libdraw side of the plan9 platform: plain C types,
 * so that both sides (APE's libdraw, GLFW's headers) include it.
 */
int	_glfwDrawInitPlan9(void);	/* the display and the process's window: 0, -1 */
void	_glfwDrawClosePlan9(void);
int	_glfwDrawAttachPlan9(void);	/* the window again (resized: rio gives it a new name) */
int	_glfwDrawAgainPlan9(void);	/* the window's wsys gone: the window in the next one: 0, -1 (the window went) */
int	_glfwDrawReconnectPlan9(void);	/* draw(3) anew after _glfwDrawAgainPlan9 */
/* the window's contents on the screen, and the border around them */
void	_glfwDrawRectPlan9(int *x, int *y, int *w, int *h, int *border);
void	_glfwDrawScreenPlan9(int *w, int *h);	/* the whole screen */
int	_glfwDrawDpiPlan9(void);	/* the window system's dots per inch (wsys' scale ×96), as of the last attach */
