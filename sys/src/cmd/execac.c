/*
 * NIX: run a command on an application core (execac(2)).
 */
#include <u.h>
#include <libc.h>

void
usage(void)
{
	fprint(2, "usage: %s [-c core] cmd [arg ...]\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	char *cmd;
	int core;

	core = -1;
	ARGBEGIN{
	case 'c':
		core = atoi(EARGF(usage()));
		break;
	default:
		usage();
	}ARGEND
	if(argc < 1)
		usage();
	cmd = argv[0];
	if(cmd[0] != '/' && strncmp(cmd, "./", 2) != 0 && strncmp(cmd, "../", 3) != 0)
		cmd = smprint("/bin/%s", cmd);
	execac(core, cmd, argv);
	sysfatal("execac %s: %r", cmd);
}
