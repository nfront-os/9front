/*
 * drawcmp: draw(3) operations into images off the screen, each result's
 * bytes (unloadimage) as a SHA-1: run against the kernel's devdraw and
 * against drawfs (tests/draw/run.sh), the two outputs must be the same.
 * Fills, masks (k1, k8), replicated sources, clipping, the compositing
 * operators, alpha, text in the default font, lines, ellipses, arcs,
 * polygons, load and cload in several channels, a screen of windows on
 * an image (layers: drawn into, raised, lowered, moved, freed).
 * Output: "name sha1" lines, then "drawcmp: done".
 */
#include <u.h>
#include <libc.h>
#include <draw.h>
#include <libsec.h>

static void
emit(char *name, Image *i)
{
	uchar *buf, dig[SHA1dlen];
	int n, len;

	len = Dy(i->r)*bytesperline(i->r, i->depth);
	buf = malloc(len);
	if(buf == nil)
		sysfatal("malloc: %r");
	n = unloadimage(i, i->r, buf, len);
	if(n != len){
		print("%s unload %d/%d %r\n", name, n, len);
		free(buf);
		return;
	}
	sha1(buf, len, dig, nil);
	print("%s %.*lH\n", name, SHA1dlen, dig);
	free(buf);
}

static Image*
mk(Rectangle r, ulong chan, int repl, ulong col)
{
	Image *i;

	if((i = allocimage(display, r, chan, repl, col)) == nil)
		sysfatal("allocimage %R %s: %r", r, chantostr((char[16]){0}, chan));
	return i;
}

/* a pattern of its own in every byte of an image */
static void
pattern(Image *i, int seed)
{
	uchar *buf;
	int k, len;

	len = Dy(i->r)*bytesperline(i->r, i->depth);
	buf = malloc(len);
	for(k = 0; k < len; k++)
		buf[k] = (k*31 + seed*17 + (k>>7)*5) ^ (k>>3);
	if(loadimage(i, i->r, buf, len) != len)
		sysfatal("loadimage: %r");
	free(buf);
}

void
main(int argc, char **argv)
{
	Image *dst, *src, *alpha, *k1, *k8, *repl, *rgba, *cmap, *g2, *back, *w1, *w2, *w3;
	Screen *scr;
	Rectangle r;
	Point pts[6];
	int op;
	char name[32];
	static char *opname[] = {
		[Clear] "clear", [SinD] "sind", [DinS] "dins", [SoutD] "soutd", [DoutS] "douts",
		[S] "s", [SoverD] "soverd", [SatopD] "satopd", [SxorD] "sxord", [D] "d",
		[DoverS] "dovers", [DatopS] "datops",
	};

	ARGBEGIN{
	}ARGEND
	USED(argc, argv);
	fmtinstall('H', encodefmt);
	if(initdraw(nil, nil, "drawcmp") < 0)
		sysfatal("initdraw: %r");

	r = Rect(0, 0, 211, 137);	/* odd sizes: no word alignment by chance */
	dst = mk(r, RGB24, 0, DWhite);
	emit("alloc", dst);

	/* fills */
	draw(dst, Rect(10, 10, 100, 60), display->black, nil, ZP);
	src = mk(Rect(0, 0, 1, 1), RGB24, 1, 0x336699FF);
	draw(dst, Rect(50, 30, 180, 120), src, nil, ZP);
	emit("fill", dst);

	/* a pattern, copies, overlapping copies */
	src = mk(Rect(3, 5, 103, 85), RGB24, 0, DNofill);
	pattern(src, 1);
	emit("load", src);
	draw(dst, Rect(20, 15, 120, 95), src, nil, Pt(3, 5));
	draw(dst, Rect(30, 25, 130, 105), dst, nil, Pt(20, 15));
	emit("copy", dst);

	/* alpha */
	alpha = mk(Rect(0, 0, 64, 64), ARGB32, 0, DNofill);
	pattern(alpha, 2);
	draw(dst, Rect(100, 40, 164, 104), alpha, nil, ZP);
	emit("argb32", dst);
	rgba = mk(Rect(0, 0, 1, 1), RGBA32, 1, 0x80402080);
	draw(dst, Rect(0, 0, 211, 50), rgba, nil, ZP);
	emit("rgba32repl", dst);

	/* masks */
	k1 = mk(Rect(0, 0, 70, 40), GREY1, 0, DNofill);
	pattern(k1, 3);
	k8 = mk(Rect(0, 0, 70, 40), GREY8, 0, DNofill);
	pattern(k8, 4);
	draw(dst, Rect(5, 80, 75, 120), display->black, k1, ZP);
	draw(dst, Rect(80, 80, 150, 120), src, k8, Pt(10, 10));
	emit("masks", dst);

	/* a replicated source, clipped */
	repl = mk(Rect(0, 0, 7, 5), RGB24, 1, DNofill);
	pattern(repl, 5);
	replclipr(dst, 0, Rect(15, 15, 190, 125));
	draw(dst, dst->r, repl, nil, Pt(2, 3));
	replclipr(dst, 0, dst->r);
	emit("repl", dst);

	/* the operators */
	for(op = Clear; op <= SoverD; op++){
		if(opname[op] == nil)
			continue;
		draw(dst, dst->r, display->white, nil, ZP);
		draw(dst, Rect(0, 0, 120, 137), src, nil, Pt(3, 5));
		drawop(dst, Rect(40, 20, 200, 120), alpha, k8, ZP, op);
		snprint(name, sizeof name, "op.%s", opname[op]);
		emit(name, dst);
	}

	/* text */
	draw(dst, dst->r, display->white, nil, ZP);
	string(dst, Pt(4, 4), display->black, ZP, font, "The quick brown fox, 0123 ÄÖÜ");
	stringbg(dst, Pt(4, 30), src, Pt(3, 5), font, "jumps over", display->black, ZP);
	runestring(dst, Pt(4, 60), alpha, ZP, font, L"the lazy dog. αβγ");
	emit("text", dst);

	/* lines, curves, polygons */
	draw(dst, dst->r, display->white, nil, ZP);
	line(dst, Pt(5, 5), Pt(200, 130), Enddisc, Endarrow, 3, display->black, ZP);
	line(dst, Pt(200, 5), Pt(5, 130), Endsquare, Endsquare, 0, src, ZP);
	ellipse(dst, Pt(105, 68), 60, 40, 2, alpha, ZP);
	fillellipse(dst, Pt(60, 40), 25, 15, src, Pt(3, 5));
	arc(dst, Pt(150, 90), 30, 30, 1, display->black, ZP, 30, 240);
	fillarc(dst, Pt(40, 100), 20, 25, src, ZP, 0, 270);
	pts[0] = Pt(120, 10); pts[1] = Pt(200, 30); pts[2] = Pt(180, 120);
	pts[3] = Pt(130, 80); pts[4] = Pt(110, 120); pts[5] = Pt(120, 10);
	poly(dst, pts, 6, Endsquare, Endsquare, 1, display->black, ZP);
	fillpoly(dst, pts, 5, 0, alpha, ZP);
	bezier(dst, Pt(10, 120), Pt(50, 10), Pt(150, 130), Pt(205, 20), Enddisc, Enddisc, 1, display->black, ZP);
	emit("curves", dst);

	/* channels */
	cmap = mk(Rect(0, 0, 50, 30), CMAP8, 0, DNofill);
	pattern(cmap, 6);
	g2 = mk(Rect(0, 0, 50, 30), GREY2, 0, DNofill);
	pattern(g2, 7);
	draw(dst, Rect(0, 0, 50, 30), cmap, nil, ZP);
	draw(dst, Rect(60, 0, 110, 30), g2, nil, ZP);
	draw(cmap, cmap->r, src, nil, Pt(10, 10));
	emit("cmap8", cmap);
	emit("channels", dst);

	/* cload: the compressed form of an image, loaded */
	{
		uchar *buf;
		int n, len;
		Image *c;

		len = Dy(src->r)*bytesperline(src->r, src->depth);
		buf = malloc(len);
		n = unloadimage(src, src->r, buf, len);
		c = mk(src->r, RGB24, 0, DNofill);
		if(n == len && loadimage(c, c->r, buf, len) == len)
			emit("reload", c);
		free(buf);
	}

	/* a screen of windows on an image */
	back = mk(Rect(0, 0, 180, 140), RGB24, 0, DNofill);
	draw(back, back->r, display->white, nil, ZP);
	scr = allocscreen(back, src, 0);
	if(scr == nil)
		sysfatal("allocscreen: %r");
	w1 = allocwindow(scr, Rect(10, 10, 100, 80), Refbackup, 0xFF0000FF);
	w2 = allocwindow(scr, Rect(50, 40, 160, 120), Refnone, 0x00FF00FF);
	w3 = allocwindow(scr, Rect(30, 60, 90, 130), Refbackup, 0x0000FFFF);
	string(w1, Pt(15, 15), display->black, ZP, font, "w1");
	draw(w2, w2->r, alpha, nil, ZP);
	emit("windows", back);
	topwindow(w1);
	emit("top", back);
	bottomwindow(w3);
	emit("bottom", back);
	originwindow(w2, Pt(0, 0), Pt(70, 50));
	emit("origin", back);
	freeimage(w1);
	emit("free", back);
	freeimage(w2);
	freeimage(w3);
	freescreen(scr);
	emit("freescreen", back);

	flushimage(display, 1);
	print("drawcmp: done\n");
	exits(nil);
}
