/*
 * SPDX-License-Identifier: MIT
 *
 * LinuxKPI for 9front, the Plan 9 side: the machine's SMBIOS tables
 * (#P/smbios: the entry point, then the structure table; the kernel
 * copies them at boot, pc/smbios.c), read once.  The Linux side
 * (kpi/linux/dmi.c) knows which structure and field each of Linux's
 * DMI strings is; here only structures, their bytes and strings.
 */
#include <u.h>
#include <libc.h>
#include "../include/kpi/p9.h"

static Lock lk;
static int loaded;
static uchar *tab;	/* the structure table */
static int ntab;
static int version;	/* major<<8 | minor */

static void
load(void)
{
	uchar *b, *e;
	int fd, n, m, len;

	lock(&lk);
	if(loaded){
		unlock(&lk);
		return;
	}
	loaded = 1;
	b = nil;
	n = 0;
	if((fd = open("#P/smbios", OREAD)) >= 0){
		for(;;){
			b = realloc(b, n + 8192);
			if(b == nil)
				break;
			if((m = read(fd, b + n, 8192)) <= 0)
				break;
			n += m;
		}
		close(fd);
	}
	if(b == nil || n < 0x18){
		free(b);
		unlock(&lk);
		return;
	}
	e = b;
	if(memcmp(e, "_SM3_", 5) == 0){
		len = e[6];
		version = e[7]<<8 | e[8];
	}else if(memcmp(e, "_SM_", 4) == 0){
		len = e[5];
		version = e[6]<<8 | e[7];
	}else
		len = n;
	if(len < n){
		tab = b + len;
		ntab = n - len;
	}else
		free(b);
	unlock(&lk);
}

/* the first structure of type t: its formatted part and length; nil if none */
static uchar*
find(int t, int *flen)
{
	uchar *p, *e, *s;

	load();
	if(tab == nil)
		return nil;
	p = tab;
	e = tab + ntab;
	while(p + 4 <= e && p[1] >= 4 && p + p[1] <= e){
		if(p[0] == t){
			*flen = p[1];
			return p;
		}
		if(p[0] == 127)	/* end of table */
			break;
		/* the strings after the formatted part end with two NULs */
		for(s = p + p[1]; s + 1 < e && (s[0] != 0 || s[1] != 0); s++)
			;
		p = s + 2;
	}
	return nil;
}

/* SMBIOS version, major<<8 | minor, or 0 */
int
p9_smbios_version(void)
{
	load();
	return version;
}

/*
 * the string the byte at offset off of the first structure of type t
 * names (SMBIOS strings count from 1; 0 is none), or nil
 */
const char*
p9_smbios_string(int t, int off)
{
	uchar *p, *s, *e;
	int flen, i;

	if((p = find(t, &flen)) == nil || off >= flen || p[off] == 0)
		return nil;
	e = tab + ntab;
	s = p + flen;
	for(i = 1; i < p[off] && s < e; i++){
		while(s < e && *s != 0)
			s++;
		if(s + 1 >= e || s[1] == 0)
			return nil;	/* the strings end before */
		s++;
	}
	if(s >= e || *s == 0)
		return nil;
	for(p = s; p < e && *p != 0; p++)
		;
	if(p >= e)
		return nil;	/* not terminated */
	return (char*)s;
}

/* n bytes from offset off of the first structure of type t; how many there were */
int
p9_smbios_bytes(int t, int off, void *buf, int n)
{
	uchar *p;
	int flen;

	if((p = find(t, &flen)) == nil || off >= flen)
		return 0;
	if(n > flen - off)
		n = flen - off;
	memmove(buf, p + off, n);
	return n;
}
