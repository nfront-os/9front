/*
 * An ethernet /dev/null.
 * Useful as a bridging target with ethernet-based VPN.
 */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"
#include "../port/error.h"
#include "../port/netif.h"
#include "../port/etherif.h"

static long
ctl(Ether *ether, void *buf, long n)
{
	uchar ea[Eaddrlen];
	Cmdbuf *cb;

	cb = parsecmd(buf, n);
	if(cb->nf >= 2
	&& strcmp(cb->f[0], "ea")==0
	&& parseether(ea, cb->f[1]) == 0){
		free(cb);
		memmove(ether->ea, ea, Eaddrlen);
		memmove(ether->Netif.addr, ether->ea, Eaddrlen);
		return 0;
	}
	free(cb);
	error(Ebadctl);
}

static void
attach(Ether *ether)
{
	/* silently discard output */
	qnoblock(ether->oq, 1);
	qsetlimit(ether->oq, 0);
}

static void
multicast(void *, uchar*, int)
{
}

static void
promiscuous(void *, int)
{
}

static int
reset(Ether* ether)
{
	if(ether->ISAConf.type==nil)
		return -1;
	ether->Netif.link = 0;
	ether->Netif.mbps = 1000;
	ether->attach = attach;
	ether->Netif.multicast = multicast;
	ether->Netif.promiscuous = promiscuous;
	ether->ctl = ctl;
	ether->Netif.arg = ether;
	return 0;
}

void
ethersinklink(void)
{
	addethercard("sink", reset);
}
