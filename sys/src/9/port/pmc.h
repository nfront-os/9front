/*
 * Performance counters, pmc(3): the interface between
 * port/devpmc.c and the architecture's pmcio.c; after NIX's
 * port/pmc.h and k8/dat.h (Lsub, Bell Labs, Sandia, Vita Nuova;
 * MIT license, see /lib/legal/nix).
 *
 * The pmcio.c functions below, but pmcarchinit, pmcdescstr and
 * pmctrans, work on the counters of the processor they run on,
 * at splhi, maybe in an interrupt handler: they must not sleep
 * or call error(). devpmc.c brings a request to the processor
 * whose counters it is about (pmcipi, pmcintr).
 */
typedef struct PmcCtl PmcCtl;
typedef struct PmcCtlCtrId PmcCtlCtrId;

enum {
	PmcCtlNullval	= 0xdead,	/* PmcCtl field: leave as it is */
	PmcMaxCtrs	= 32,		/* PMUv3 has up to 31 */
};

struct PmcCtl {
	int	enab;
	int	user;
	int	os;
	int	nodesc;			/* descstr not set */
	char	descstr[KNAMELEN];	/* configuration: "event umask" (x86), "event" (arm64) */
	int	reset;
};

struct PmcCtlCtrId {
	char	portdesc[KNAMELEN];	/* name in ctrdesc */
	char	archdesc[KNAMELEN];	/* its configuration */
};

/* pmcio.c */
int	pmcarchinit(void);		/* once; returns the number of counters */
int	pmcgetctl(PmcCtl*, int);
int	pmcsetctl(PmcCtl*, int);
int	pmcgetctr(u64int*, int);
int	pmcsetctr(u64int, int);
int	pmcdescstr(char*, int);
int	pmctrans(PmcCtl*);		/* name to configuration; -1: bad */
void	pmcipi(int);			/* interrupt processor machno: pmcintr */

/* devpmc.c */
void	pmcintr(void);
