#pragma	lib	"libui.a"
#pragma	src	"/sys/src/libui"

/*
 * ui(2): a client of ui(4), a window of widgets described through files
 */
typedef struct Ui Ui;
typedef struct Uievent Uievent;

struct Ui
{
	char	*mtpt;		/* where ui's files are mounted */
	char	*srv;		/* its /srv entry's name */
	int	ctl;		/* mtpt/ctl, written */
	int	ev;		/* mtpt/event, read */
	char	buf[8192];	/* what event gave, not yet taken */
	int	nbuf;
};

struct Uievent
{
	char	*kind;		/* click text enter check slide close */
	char	*id;		/* the node's (nil for close) */
	char	*arg;		/* the rest of the line ("" if none) */
	char	line[1024];
};

Ui*	uiopen(char *srv, char *mtpt);
Ui*	uinew(char *title, char *mtpt);
int	uictl(Ui *u, char *fmt, ...);
int	uiset(Ui *u, char *id, char *key, char *value);
int	uievent(Ui *u, Uievent *e);
void	uiclose(Ui *u);

#pragma	varargck	argpos	uictl	2
