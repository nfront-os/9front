/* posix */
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/* socket extensions */
#include <sys/uio.h>
#include <sys/socket.h>

#include "priv.h"
#include "lib.h"
#include "sys9.h"

/* we can't avoid overrunning npath because we don't know how big it is. */
void
_sock_srvname(char *npath, char *path)
{
	char *p;

	strcpy(npath, "/srv/UD.");
	p = strrchr(path, '/');
	if(p == 0)
		p = path;
	else
		p++;
	strcat(npath, p);
}

int
_sock_srv(char *path, int fd)
{
	int sfd;
	char msg[8+256+1];

	/* change the path to something in srv */
	_sock_srvname(msg, path);

	/* remove any previous instance */
	unlink(msg);

	/*
	 * put the fd in /srv; 9front-ng: the entry stays while this process
	 * does (ORCLOSE on a descriptor kept open, not the programs it
	 * execs: OCEXEC), as a listening socket's file names a live listener
	 * (a Wayland server's was left after it exited)
	 */
	sfd = _CREATE(msg, OWRITE|ORCLOSE|OCEXEC, 0666);
	if(sfd < 0){
		_syserrno();
		close(fd);
		return -1;
	}
	snprintf(msg, sizeof msg, "%d", fd);
	if(_WRITE(sfd, msg, strlen(msg)) < 0){
		_syserrno();
		_CLOSE(sfd);
		close(fd);
		return -1;
	}
	close(fd);
	return 0;
}
