/*
 * NIX tubes as seen by the kernel, after NIX port/tube.h
 * (Lsub, Bell Labs, Sandia, Vita Nuova; MIT license, see /lib/legal/nix).
 * Tube is the user's (/sys/include/tube.h); the kernel works on it
 * through a KTube, which holds the Sems of its two semaphores.
 */
typedef struct Tube Tube;
typedef struct KTube KTube;
typedef struct KTalt KTalt;

/*
 * [K]Talt.op
 */
enum
{
	TSND = 0x6666,	/* weird numbers, for debugging */
	TRCV,
	TNBSND,
	TNBRCV,
	TNOP
};

struct Tube
{
	int	msz;	/* message size */
	int	tsz;	/* tube size (# of messages) */
	int	nmsg;	/* semaphore: # of messages in tube */
	int	nhole;	/* semaphore: # of free slots in tube */
	int	hd;
	int	tl;
};

struct KTube
{
	Tube	*t;
	int	msz;	/* t->msz when made: the size of the kernel's messages */
	Usem	*hole;
	Usem	*msg;
};

struct KTalt
{
	KTube	*kt;
	void	*m;
	int	op;
};

extern void	freektube(KTube *t);
extern int	nbtrecv(KTube *t, void *p);
extern int	nbtsend(KTube *t, void *p);
extern int	talt(KTalt a[], int na);
extern void	trecv(KTube *t, void *p);
extern void	tsend(KTube *t, void *p);
extern KTube*	u2ktube(Tube *t);
extern void	upsem(Usem*);
extern int	downsem(Usem*, int);
extern int	altsems(Usem*[], int);
