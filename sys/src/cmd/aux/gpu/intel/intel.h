/*
 * SPDX-License-Identifier: MIT
 *
 * what intelgpu.c and display.c share
 */

/* intelgpu.c: MMIO (forcewake taken for the GT's registers) */
u32int	rd(ulong);
void	wr(ulong, u32int);

/* display.c: the outputs, read from the hardware as the firmware or aux/vga set them up */
typedef struct Output Output;
struct Output {
	char	name[16];	/* eDP-1, DP-1, HDMI-A-1: Linux's names */
	int	port;		/* DDI A-D: 0-3 */
	int	type;		/* Oedp, Odp, Ohdmi */
	int	connected;
	uchar	edid[256];
	int	nedid;
	int	trans;		/* the transcoder driving it (Tedp, 0-2), -1 none */
	int	pipe;		/* the pipe feeding that transcoder, -1 none */
	int	w, h;		/* its active size; refresh in mHz */
	long	refresh;
};
enum { Oedp, Odp, Ohdmi };
enum { Tedp = 3 };
enum { Maxout = 8 };

extern Output	outputs[Maxout];
extern int	noutputs;

void	dispinit(void);
char*	dispstatus(void);
char*	outmodes(Output*);
char*	outmode(Output*);
