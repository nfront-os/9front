/*
 * SPDX-License-Identifier: MIT
 *
 * aux/intelgpu: the file server: gpu(4)'s files.
 * Plan 9 only: no counterpart in Linux.
 */
#include <u.h>
#include <libc.h>
#include <fcall.h>
#include <thread.h>
#include <9p.h>
#include "../i915_drv.h"

File *ctxdir;
Srv fs;

int
ftype(File *f)
{
	uintptr a;

	a = (uintptr)f->aux;
	return a >= Xctxbase ? a % Xctxbase : a;
}

Ctx*
fctx(File *f)
{
	uintptr a;

	a = (uintptr)f->aux;
	if(a < Xctxbase)
		return nil;
	return ctxs[a/Xctxbase - 1];
}

/*
 * contexts and their objects
 */
char*
ctxreply(Ctx *c, char *fmt, ...)
{
	va_list arg;

	free(c->reply);
	va_start(arg, fmt);
	c->reply = vsmprint(fmt, arg);
	va_end(arg);
	return nil;
}

char*
planestr(void)
{
	ulong surf, stride;
	int p, bpp, w, h, tiled;

	if((p = plane(&surf, &stride, &bpp, &w, &h, &tiled)) < 0)
		return estrdup9p("plane none\n");
	return smprint("plane %c %#lux %lud %d %d %d%s\n", 'A'+p, surf, stride, bpp, w, h,
		tiled ? " tiled" : "");
}

char*
statusstr(void)
{
	char buf[1024], *s, *t, *u, *id;
	Engine *e;
	Ectx *q;
	Hole *h;
	ulong nfree;
	int i, n;

	id = ctlline("id", buf, sizeof buf);
	s = smprint("device intel %s, gen 8\nirq %s %lud\nobjects %s, %s\n", id ? id : "?",
		irqfd >= 0 ? "on" : "poll", nirq, uncached ? "uncached" : "cached (llc)",
		global ? "global gtt" : "ppgtt per context");
	qlock(&lk);
	for(i = 0; i < nelem(eng); i++){
		e = &eng[i];
		n = 0;
		for(q = e->queue; q != nil; q = q->next)
			n++;
		t = smprint("%sengine %s %s execlists: running ctx %d, %d queued, %ud switches, csb %d/%d,"
			" status %#ux mode %#ux acthd %#ux eir %#ux ipehr %#ux pdp0 %#.8ux%.8ux resets %d\n",
			s, e->name, e->ok ? "ok" : "off", e->running ? e->running->ctx->id : 0, n, e->nswitch,
			e->csbread, rd(RING_CONTEXT_STATUS_PTR(e->base)) & GEN8_CSB_PTR_MASK, rd(RING_EXECLIST_STATUS_LO(e->base)), rd(RING_MODE_GEN7(e->base)),
			rd(RING_ACTHD(e->base)), rd(RING_EIR(e->base)), rd(RING_IPEHR(e->base)), rd(GEN8_RING_PDP_UDW(e->base, 0)),
			rd(GEN8_RING_PDP_LDW(e->base, 0)), e->nreset);
		free(s);
		s = t;
	}
	/* the render engine's workaround registers as they read now (the last context's) */
	n = 0;
	for(i = 0; i < nrcswa; i++)
		if((rd(rcswa[i].reg) & rcswa[i].val>>16) == (rcswa[i].val & 0xFFFF))
			n++;
	t = smprint("%srcs workarounds %d/%d\nsseu slices %#x subslices %#x eus %d\n"
		"power rc6 %s %llud ms, %d MHz (asked %d, min %d, rp1 %d, max %d)%s\n", s, n, nrcswa,
		sslices, ssubslices, seus,
		rps.on ? "on" : "off", (uvlong)rd(GEN6_GT_GFX_RC6)*128/100000,
		(rd(GEN6_RPSTAT1)>>7 & 0x7F)*50, rps.cur*50, rps.min*50, rps.rp1*50, rps.max*50,
		fwheld ? ", forcewake" : "");
	free(s);
	s = t;
	nfree = 0;
	for(h = holes; h != nil; h = h->next)
		nfree += h->len;
	qunlock(&lk);
	n = 0;
	for(i = 0; i < Nbo; i++)
		if(bos[i] != nil)
			n++;
	t = smprint("%sgtt %lld entries, %lud MB free, %d objects\n", s, nent, nfree>>20, n);
	free(s);
	s = t;
	if(scan.bo != nil)
		t = smprint("%sscanout ctx %d bo %d gtt %#lux\n", s, scan.bo->ctx->id, scan.bo->id, scan.bo->scangtt);
	else
		t = smprint("%sscanout none\n", s);
	free(s);
	s = t;
	for(i = 0; i < Nctx; i++)
		if(ctxs[i] != nil){
			t = smprint("%sctx %d fence %llud/%llud seqno rcs %ud/%ud bcs %ud/%ud lrca %#lux pd0 %#llux%s\n",
				s, ctxs[i]->id, ctxs[i]->donefence, ctxs[i]->nfence,
				ecdone(&ctxs[i]->ec[0]), ctxs[i]->ec[0].nseq, ecdone(&ctxs[i]->ec[1]), ctxs[i]->ec[1].nseq,
				ctxs[i]->ec[0].lrca, pdpval(ctxs[i], 0), ctxs[i]->failed ? " failed" : "");
			free(s);
			s = t;
		}
	t = smprint("%s%s", s, u = dispstatus());
	free(u);
	free(s);
	s = t;
	return s;
}

void
fsopen(Req *r)
{
	Fidaux *fa;
	Ctx *c;

	switch(ftype(r->fid->file)){
	case Xclone:
		qlock(&ctxlock);
		if((c = newctx()) == nil){
			qunlock(&ctxlock);
			responderror(r);
			return;
		}
		closefile(r->fid->file);
		r->fid->file = c->ctl;
		incref(c->ctl);
		r->ofcall.qid = c->ctl->qid;
		c->ref++;
		qunlock(&ctxlock);
		break;
	case Xctxctl:
	case Xctxsubmit:
	case Xctxfence:
		qlock(&ctxlock);
		if((c = fctx(r->fid->file)) == nil){
			qunlock(&ctxlock);
			respond(r, "context gone");
			return;
		}
		c->ref++;
		qunlock(&ctxlock);
		break;
	default:
		respond(r, nil);
		return;
	}
	fa = emalloc9p(sizeof *fa);
	memset(fa, 0, sizeof *fa);
	fa->ctx = c;
	r->fid->aux = fa;
	respond(r, nil);
}

void
fsdestroyfid(Fid *f)
{
	Fidaux *fa;
	Ctx *c;

	if((fa = f->aux) == nil)
		return;
	f->aux = nil;
	c = fa->ctx;
	free(fa);
	qlock(&ctxlock);
	if(--c->ref == 0)
		freectx(c);
	qunlock(&ctxlock);
}

void
fsread(Req *r)
{
	Fidaux *fa;
	Ctx *c;
	char *s;
	int n;

	fa = r->fid->aux;
	n = ftype(r->fid->file);
	if(n >= Xout && n < Xctxbase){
		Output *o;

		o = &outputs[(n - Xout)>>4];
		switch(n & 0xF){
		case Xoutedid:
			readbuf(r, o->edid, o->nedid);
			respond(r, nil);
			return;
		case Xoutmodes:
			s = outmodes(o);
			break;
		default:
			s = outmode(o);
			break;
		}
		readstr(r, s);
		free(s);
		respond(r, nil);
		return;
	}
	switch(n){
	case Xctl:
		s = planestr();
		break;
	case Xstatus:
		qlock(&ctxlock);
		s = statusstr();
		qunlock(&ctxlock);
		break;
	case Xctxctl:
		c = fa->ctx;
		qlock(&ctxlock);
		s = estrdup9p(c->reply != nil ? c->reply : "");
		qunlock(&ctxlock);
		break;
	case Xctxfence:
		c = fa->ctx;
		r->ifcall.offset = 0;
		if(fa->fence == 0){
			qlock(&lk);
			reap();
			s = smprint("%llud %llud%s\n", c->donefence, c->nfence, c->failed ? " failed" : "");
			qunlock(&lk);
			break;
		}
		if(fa->fence > c->nfence){
			respond(r, "no such fence");
			return;
		}
		if(!fa->query){
			srvrelease(r->srv);
			fencewait(c, fa->fence);
			srvacquire(r->srv);
		}
		qlock(&lk);
		n = fencedone(c, fa->fence);
		qunlock(&lk);
		s = smprint("%llud %s%s\n", fa->fence, n ? "done" : "busy", c->failed ? " failed" : "");
		break;
	default:
		respond(r, "bug");
		return;
	}
	readstr(r, s);
	free(s);
	respond(r, nil);
}

/* the server ends: the draw screen back on the display */
void
fsend(Srv*)
{
	qlock(&lk);
	scanrestore();
	qunlock(&lk);
}

char*
ctxctl(Ctx *c, char **f, int n)
{
	Bo *b;

	if(n == 2 && strcmp(f[0], "bo") == 0)
		return bonew(c, strtoull(f[1], nil, 0), Anyaddr);
	if(n == 3 && strcmp(f[0], "bo") == 0)
		return bonew(c, strtoull(f[1], nil, 0), strtoull(f[2], nil, 0));
	if(n == 3 && strcmp(f[0], "bind") == 0){
		if((b = findbo(c, atoi(f[1]))) == nil)
			return "unknown object";
		return bobind(b, strtoull(f[2], nil, 0));
	}
	if(n == 2 && strcmp(f[0], "map") == 0 && strcmp(f[1], "plane") == 0)
		return mapplane(c);
	if(n == 2 && strcmp(f[0], "scanout") == 0 && atoi(f[1]) == 0)
		return scanout(c, 0, 0, 0, 0);
	if(n == 5 && strcmp(f[0], "scanout") == 0)
		return scanout(c, atoi(f[1]), atoi(f[2]), atoi(f[3]), atoi(f[4]));
	/* close: gpu(4)'s word of every driver for letting go of an object */
	if(n == 2 && (strcmp(f[0], "free") == 0 || strcmp(f[0], "close") == 0)){
		if((b = findbo(c, atoi(f[1]))) == nil)
			return "unknown object";
		if(c->nfence > c->donefence)
			fencewait(c, c->nfence);	/* the GPU may still use it */
		bofree(b);
		return nil;
	}
	return "unknown ctl message";
}

/* the device's ctl: how the PDPs are loaded, engine resets */
char*
devctl(char **f, int n)
{
	int i;

	if(n == 1 && strcmp(f[0], "reset") == 0){
		qlock(&lk);
		for(i = 0; i < nelem(eng); i++)
			engreset(&eng[i]);
		gen8_irq_postinstall();
		for(i = 0; i < nelem(eng); i++)
			execlists_dequeue(&eng[i]);
		reap();
		rwakeupall(&done);
		qunlock(&lk);
		return nil;
	}
	return "usage: reset";
}

void
fswrite(Req *r)
{
	char buf[256], *f[8], *e;
	Fidaux *fa;
	ulong flip;
	int n;

	n = r->ifcall.count;
	if(n >= sizeof buf)
		n = sizeof buf-1;
	memmove(buf, r->ifcall.data, n);
	buf[n] = 0;
	fa = r->fid->aux;
	if((n = ftype(r->fid->file)) >= Xout && n < Xctxbase && (n & 0xF) == Xoutmode){
		qlock(&ctxlock);
		e = outsetmode(&outputs[(n - Xout)>>4], buf);
		qunlock(&ctxlock);
		if(e != nil){
			respond(r, e);
			return;
		}
		r->ofcall.count = r->ifcall.count;
		respond(r, nil);
		return;
	}
	n = tokenize(buf, f, nelem(f));
	switch(ftype(r->fid->file)){
	case Xctl:
		e = devctl(f, n);
		break;
	case Xctxctl:
		if(n < 1){
			respond(r, "empty ctl message");
			return;
		}
		qlock(&ctxlock);
		scan.flip = 0;
		e = ctxctl(fa->ctx, f, n);
		flip = scan.flip;
		scan.flip = 0;
		qunlock(&ctxlock);
		if(e == nil && flip != 0)
			flipwait(r, flip, scan.p);
		break;
	case Xctxsubmit:
		if(n != 2 && n != 3){
			respond(r, "usage: bo offset [rcs|bcs]");
			return;
		}
		qlock(&ctxlock);
		e = submit(fa->ctx, atoi(f[0]), strtoul(f[1], nil, 0), n == 3 ? f[2] : "rcs");
		qunlock(&ctxlock);
		break;
	case Xctxfence:
		if(n == 1){
			fa->query = 0;
			fa->fence = strtoull(f[0], nil, 0);
		}else if(n == 2 && (strcmp(f[0], "wait") == 0 || strcmp(f[0], "query") == 0)){
			fa->query = f[0][0] == 'q';
			fa->fence = strtoull(f[1], nil, 0);
		}else{
			respond(r, "usage: [wait|query] fence");
			return;
		}
		e = nil;
		break;
	default:
		e = "permission denied";
	}
	if(e != nil){
		respond(r, e);
		return;
	}
	r->ofcall.count = r->ifcall.count;
	respond(r, nil);
}

void
mktree(void)
{
	File *gpu, *n, *o;
	char *user;
	int i;

	user = getuser();
	fs.tree = alloctree(user, user, DMDIR|0555, nil);
	gpu = createfile(fs.tree->root, "gpu", user, DMDIR|0555, nil);
	n = createfile(gpu, "0", user, DMDIR|0555, nil);
	createfile(n, "ctl", user, 0664, (void*)Xctl);
	createfile(n, "status", user, 0444, (void*)Xstatus);
	createfile(n, "clone", user, 0666, (void*)Xclone);
	ctxdir = createfile(n, "ctx", user, DMDIR|0555, nil);
	n = createfile(n, "outputs", user, DMDIR|0555, nil);
	for(i = 0; i < noutputs; i++){
		o = createfile(n, outputs[i].name, user, DMDIR|0555, nil);
		createfile(o, "edid", user, 0444, (void*)(uintptr)(Xout | i<<4 | Xoutedid));
		createfile(o, "modes", user, 0444, (void*)(uintptr)(Xout | i<<4 | Xoutmodes));
		createfile(o, "mode", user, 0664, (void*)(uintptr)(Xout | i<<4 | Xoutmode));
	}
}
