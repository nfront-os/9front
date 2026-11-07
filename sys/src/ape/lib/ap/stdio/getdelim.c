/*
 * 9front-ng: getdelim and getline (POSIX.1-2008): a line (up to delim,
 * which it keeps) into *linep, made bigger with realloc as needed and
 * null-terminated; its length, or -1 at the end of the file or on an
 * error (errno).  wayland-cursor reads its themes' index files so.
 */
#include "iolib.h"
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>

ssize_t getdelim(char **linep, size_t *np, int delim, FILE *f){
	size_t len, m;
	char *p;
	int c;

	if(linep==NULL || np==NULL || f==NULL){
		errno=EINVAL;
		return -1;
	}
	if(*linep==NULL)
		*np=0;
	len=0;
	for(;;){
		if(len+1>=*np){
			m=*np<64? 64 : 2**np;
			if((p=realloc(*linep, m))==NULL){
				errno=ENOMEM;
				return -1;
			}
			*linep=p;
			*np=m;
		}
		if((c=getc(f))==EOF)
			break;
		(*linep)[len++]=c;
		if(c==delim)
			break;
	}
	(*linep)[len]='\0';
	if(len==0)
		return -1;
	return len;
}

ssize_t getline(char **linep, size_t *np, FILE *f){
	return getdelim(linep, np, '\n', f);
}
