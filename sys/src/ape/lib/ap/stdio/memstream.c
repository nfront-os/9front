/*
 * 9front-ng: open_memstream (POSIX.1-2008): a string stream (sopenw)
 * whose buffer grows as it is written; fflush and fclose put it, with a
 * null after its contents, in *bufp and its length in *sizep.  After
 * fclose the buffer is the caller's (free).
 */
#include "iolib.h"
#include <string.h>

static struct {
	char	**bufp;
	size_t	*sizep;
} ms[FOPEN_MAX];

FILE *open_memstream(char **bufp, size_t *sizep){
	FILE *f;

	if(bufp==NULL || sizep==NULL)
		return NULL;
	if((f=_IO_sopenw())==NULL)
		return NULL;
	f->flags|=MEMSTR;
	ms[f-_IO_stream].bufp=bufp;
	ms[f-_IO_stream].sizep=sizep;
	*bufp=NULL;
	*sizep=0;
	return f;
}

int _IO_memsync(FILE *f){
	char *t;
	size_t n;

	if(f->state==OPEN && _IO_setvbuf(f)!=0)
		return EOF;
	if(f->state==ERR || f->buf==NULL)
		return EOF;
	n=f->wp-f->buf;
	if(n>=f->bufl){	/* room for the null */
		if((t=realloc(f->buf, f->bufl+BUFSIZ))==NULL){
			f->state=ERR;
			return EOF;
		}
		f->buf=t;
		f->wp=t+n;
		f->bufl+=BUFSIZ;
		f->rp=t+f->bufl;
	}
	f->buf[n]='\0';
	*ms[f-_IO_stream].bufp=f->buf;
	*ms[f-_IO_stream].sizep=n;
	return 0;
}
