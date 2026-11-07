/*
 * SPDX-License-Identifier: MIT
 *
 * wlwatch [-k] label: a Wayland client's window of wsys from the outside
 * (weston's examples: wlweston.rc, wsysrestart.rc): within 10 s a window
 * labelled label; its pixels, on a grid, not all alike (something drawn);
 * again a second later, some changed (it animates); then deleted through
 * its ctl (the client is told to close), unless -k keeps it.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

enum { Grid = 8 };

static int nfail;
static char *label;
static int winid, wx0, wy0, wx1, wy1;

static void
check(const char *what, int ok, const char *fmt, ...)
{
	va_list a;

	if(ok)
		printf("ok %s: %s\n", label, what);
	else{
		printf("FAIL %s: %s", label, what);
		if(fmt != NULL){
			printf(": ");
			va_start(a, fmt);
			vprintf(fmt, a);
			va_end(a);
		}
		printf("\n");
		nfail++;
	}
	fflush(stdout);
}

static int
readfile(char *path, char *buf, int n)
{
	int fd, m;

	if((fd = open(path, O_RDONLY)) < 0)
		return -1;
	m = read(fd, buf, n-1);
	close(fd);
	if(m < 0)
		return -1;
	buf[m] = 0;
	return m;
}

static int
findwin(void)
{
	DIR *dir;
	struct dirent *e;
	char path[128], buf[256];
	int id;

	if((dir = opendir("/mnt/wsys")) == NULL)
		return -1;
	winid = -1;
	while((e = readdir(dir)) != NULL){
		if((id = atoi(e->d_name)) <= 0)
			continue;
		snprintf(path, sizeof path, "/mnt/wsys/%d/label", id);
		if(readfile(path, buf, sizeof buf) >= 0 && strcmp(buf, label) == 0)
			winid = id;
	}
	closedir(dir);
	if(winid < 0)
		return -1;
	snprintf(path, sizeof path, "/mnt/wsys/%d/ctl", winid);
	if(readfile(path, buf, sizeof buf) < 0 ||
	   sscanf(buf, "%d %d %d %d %d", &id, &wx0, &wy0, &wx1, &wy1) != 5)
		return -1;
	return 0;
}

/* the window's pixels on a Grid×Grid grid; 0 if the screen was read */
static int
sample(long px[Grid*Grid])
{
	char hdr[61], chan[16];
	int fd, sx0, sy0, sx1, sy1, n, total, m, i, j, x, y;
	unsigned char *img, *p;

	if((fd = open("/mnt/wsys/screen", O_RDONLY)) < 0)
		return -1;
	if(read(fd, hdr, 60) != 60){
		close(fd);
		return -1;
	}
	hdr[60] = 0;
	if(sscanf(hdr, "%15s %d %d %d %d", chan, &sx0, &sy0, &sx1, &sy1) != 5 ||
	   (strcmp(chan, "x8r8g8b8") != 0 && strcmp(chan, "a8r8g8b8") != 0)){
		close(fd);
		return -1;
	}
	total = (sx1 - sx0) * (sy1 - sy0) * 4;
	if((img = malloc(total)) == NULL){
		close(fd);
		return -1;
	}
	for(n = 0; n < total; n += m)
		if((m = read(fd, img + n, total - n)) <= 0)
			break;
	close(fd);
	if(n < total){
		free(img);
		return -1;
	}
	for(i = 0; i < Grid; i++)
		for(j = 0; j < Grid; j++){
			x = wx0 + (wx1 - wx0) * (2*j + 1) / (2*Grid);
			y = wy0 + (wy1 - wy0) * (2*i + 1) / (2*Grid);
			p = img + ((long)(y - sy0) * (sx1 - sx0) + (x - sx0)) * 4;
			px[i*Grid + j] = (long)p[2]<<16 | p[1]<<8 | p[0];
		}
	free(img);
	return 0;
}

int
main(int argc, char **argv)
{
	long a[Grid*Grid], b[Grid*Grid];
	char path[64];
	int i, n, fd, ok, keep;

	keep = argc == 3 && strcmp(argv[1], "-k") == 0;
	if(argc != 2 + keep){
		fprintf(stderr, "usage: wlwatch [-k] label\n");
		return 2;
	}
	label = argv[1 + keep];
	ok = -1;
	for(i = 0; i < 100 && (ok = findwin()) < 0; i++)
		usleep(100*1000);
	check("a window of wsys with its title", ok == 0, NULL);
	if(ok < 0){
		printf("wlwatch %s: failed\n", label);
		return 1;
	}
	printf("wlwatch %s: window %d, %dx%d\n", label, winid, wx1 - wx0, wy1 - wy0);
	usleep(500*1000);
	if(sample(a) < 0 || (sleep(1), sample(b)) < 0)
		check("the screen read", 0, NULL);
	else{
		for(n = 0, i = 1; i < Grid*Grid; i++)
			n += a[i] != a[0];
		check("something drawn (not one colour)", n > 0, "%06lx everywhere", a[0]);
		for(n = 0, i = 0; i < Grid*Grid; i++)
			n += a[i] != b[i];
		check("it moves (pixels changed in a second)", n > 0, NULL);
	}
	snprintf(path, sizeof path, "/mnt/wsys/%d/ctl", winid);
	if(!keep && (fd = open(path, O_WRONLY)) >= 0){
		check("delete to the window's ctl", write(fd, "delete", 6) == 6, NULL);
		close(fd);
	}
	printf(nfail ? "wlwatch %s: %d failed\n" : "wlwatch %s: all passed\n", label, nfail);
	return nfail != 0;
}
