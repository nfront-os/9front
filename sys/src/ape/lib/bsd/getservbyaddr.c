#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

/* bsd extensions */
#include <sys/uio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

struct servent*
getservbyport(int port, const char *proto)	/* GCC port: types as in the prototype, kencc ignored the difference */
{
	char buf[32];

	snprintf(buf, sizeof buf, "%d", port);
	return getservbyname(buf, proto);
}
