/*
 * Minimal, hand-written stand-in for ipconfig(8) + the usual
 * dial/mount dance -- configures a static IP matching QEMU's default
 * user-mode (SLIRP) network (guest 10.0.2.15/24, gateway/host
 * 10.0.2.2) and uses a 9P server running on the host (Harvey's
 * `ufs`, see MEMORY.md Teil 12/plan Phase 2.5) as the root file
 * system.
 *
 * Root handoff mirrors what sys/src/9/boot/{boot.c,bootrc} do for a
 * tcp root (bootrc's main()): post the connection as #s/boot, mount
 * it on /root, set $rootdir/$rootspec, `bind -a $rootdir /` and exec
 * /$cputype/init. init's newns() then builds the real name space from
 * the stock /lib/namespace (which itself does `mount -C /srv/boot
 * /root $rootspec` and `bind -a $rootdir /`). /root is one of
 * devroot.c's fixed "#/" entries, so it is walkable as a mount point.
 *
 * init gets -c for service=cpu and -t otherwise, as in bootrc.
 *
 * Deliberately NOT the real ipconfig(8): that's a ~4000-line DHCP/v6/
 * PPP-capable client. We only need one fixed, well-known address for
 * a QEMU dev/test loop, so hand-rolling the handful of /net writes
 * documented in ip(3)/ether(3) is far less risk than building and
 * gcc-porting the whole real thing.
 */
#include <u.h>
#include <libc.h>

static void
must(int ok, char *what)
{
	if(ok < 0)
		sysfatal("%s: %r", what);
}

static void
setenv(char *name, char *val)
{
	char buf[64];
	int fd;

	snprint(buf, sizeof buf, "#e/%s", name);
	fd = create(buf, OWRITE, 0666);
	must(fd, buf);
	must(write(fd, val, strlen(val)), buf);
	close(fd);
}

void
main(int argc, char *argv[])
{
	int ctlfd, routefd, datafd, srvfd, rtc, tfd, n;
	char buf[64];
	char *ufsaddr, init[64], kbdfs[64], **dp;
	static char *bootdevs[] = {"#¤", "#¶", "#P", "#S", "#f", "#æ", "#t", "#b", "#m", nil};
	Waitmsg *w;

	ufsaddr = "tcp!10.0.2.2!5640";
	if(argc > 1)
		ufsaddr = argv[1];

	/*
	 * port/initcode.c's startboot() binds, verbatim: scripts/boot/testinit.S
	 * stands in for initcode and execs us directly, so nobody else
	 * has done them yet (without them there is no /env, /fd, /srv).
	 */
	bind("#c", "/dev", MAFTER);
	bind("#d", "/fd", MREPL);
	bind("#e", "/env", MREPL|MCREATE);
	bind("#ec", "/env", MAFTER);
	bind("#p", "/proc", MREPL);
	bind("#s", "/srv", MREPL|MCREATE);
	bind("#σ", "/shr", MREPL);

	/* bootrc: for(i in ¤ ¶ P S f æ t b m) bind -qa '#'^$i /dev */
	for(dp = bootdevs; *dp != nil; dp++)
		bind(*dp, "/dev", MAFTER);

	/* bootrc: cat '#r/rtc' >/dev/time (usually better than 1970) */
	if((rtc = open("#r/rtc", OREAD)) >= 0){
		if((n = read(rtc, buf, sizeof buf - 1)) > 0 && (tfd = open("/dev/time", OWRITE)) >= 0){
			write(tfd, buf, n);
			close(tfd);
		}
		close(rtc);
	}

	must(bind("#I0", "/net", MREPL), "bind #I0");
	must(bind("#l0", "/net", MAFTER), "bind #l0");

	ctlfd = open("/net/ipifc/clone", ORDWR);
	must(ctlfd, "open ipifc/clone");
	must(fprint(ctlfd, "bind ether /net/ether0"), "bind ether");
	must(fprint(ctlfd, "add 10.0.2.15 255.255.255.0"), "add addr");

	routefd = open("/net/iproute", OWRITE);
	must(routefd, "open iproute");
	must(fprint(routefd, "add 0.0.0.0 0.0.0.0 10.0.2.2"), "add route");
	close(routefd);

	print("netmount: dialing %s...\n", ufsaddr);
	datafd = dial(ufsaddr, nil, nil, nil);
	must(datafd, "dial ufs");
	print("netmount: dialed, fd=%d\n", datafd);

	/* bootrc: echo -n $fd >/srv/boot */
	srvfd = create("#s/boot", OWRITE, 0666);
	must(srvfd, "create #s/boot");
	must(fprint(srvfd, "%d", datafd), "post #s/boot");
	close(srvfd);

	/* bootrc: mount -c /srv/boot /root; bind -a $rootdir / */
	must(mount(datafd, -1, "/root", MREPL|MCREATE, ""), "mount /root");
	close(datafd);
	setenv("rootdir", "/root");
	setenv("rootspec", "");
	must(bind("/root", "/", MAFTER), "bind -a /root /");
	print("netmount: root is %s\n", ufsaddr);

	/*
	 * bootrc: aux/kbdfs -q -s cons $a (a=/dev/eia0 for console=0), then
	 * the whole boot loop runs with </dev/cons. kbdfs provides the
	 * console line discipline: the kernel's #c/cons is write-only.
	 */
	snprint(kbdfs, sizeof kbdfs, "/%s/bin/aux/kbdfs", getenv("cputype"));
	switch(fork()){
	case -1:
		sysfatal("fork: %r");
	case 0:
		execl(kbdfs, "kbdfs", "-q", "-s", "cons", "/dev/eia0", nil);
		sysfatal("exec %s: %r", kbdfs);
	}
	if((w = wait()) == nil || w->msg[0] != '\0')
		sysfatal("kbdfs: %s", w != nil ? w->msg : "wait failed");
	free(w);
	close(0);
	must(open("/dev/cons", OREAD), "open /dev/cons");

	/* bootrc: exec $init */
	snprint(init, sizeof init, "/%s/init", getenv("cputype"));
	if(strcmp(getenv("service"), "cpu") == 0)
		execl(init, "init", "-c", nil);
	else
		execl(init, "init", "-t", nil);
	sysfatal("exec %s: %r", init);
}
