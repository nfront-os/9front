/*
 * SPDX-License-Identifier: MIT
 *
 * The keyboard for Wayland's clients: an XKB keymap (text, as
 * xkb_keymap_new_from_string reads it) made of kbdfs' tables (/dev/kbmap,
 * kbd(3)), and the keys of kbd(3)'s messages as its keycodes.  A k or K
 * message names the keys held by their runes in the table "none" (for a key
 * with the escape prefix, its rune in "esc"); the keycode of such a rune is
 * Linux's (evdev) code of the key's scancode, plus 8, as XKB counts.  The
 * levels of a key are its runes with shift, altgr and both; the modifiers are
 * the keys held whose runes are Kshift, Kctl, Kalt, Kmod4 and Kaltgr.  So a
 * client's xkbcommon gives what kbdfs would, layout and all.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* kbdfs' tables (kbdfs.c, layertab) */
enum {
	Lnone,
	Lshift,
	Lesc1,
	Laltgr,
	Lctl,
	Lctrlesc1,
	Lshiftesc1,
	Lshiftaltgr,
	Lmod4,
	Laltgrmod4,
	Nlayers,
	Nscan = 128,
};

static char *layertab[Nlayers] = {
	"none", "shift", "esc", "altgr", "ctl", "ctlesc", "shiftesc", "shiftaltgr", "mod4", "altgrmod4",
};

/* keyboard(2)'s runes */
enum {
	KF = 0xF000,
	Spec = 0xF800,
	Khome = KF|0x0D,
	Kup = KF|0x0E,
	Kdown = Spec|0x00,
	Kpgup = KF|0x0F,
	Kprint = KF|0x10,
	Kleft = KF|0x11,
	Kright = KF|0x12,
	Kpgdown = KF|0x13,
	Kins = KF|0x14,
	Kalt = KF|0x15,
	Kshift = KF|0x16,
	Kctl = KF|0x17,
	Kend = KF|0x18,
	Kscroll = KF|0x19,
	Ksbwd = KF|0x22,
	Ksfwd = KF|0x23,
	Kpause = KF|0x24,
	Kvoldn = KF|0x25,
	Kvolup = KF|0x26,
	Kmute = KF|0x27,
	Kbrtdn = KF|0x28,
	Kbrtup = KF|0x29,
	Kdel = 0x7f,
	Kesc = 0x1b,
	Kbreak = Spec|0x61,
	Kcaps = Spec|0x64,
	Knum = Spec|0x65,
	Kaltgr = Spec|0x67,
	Kmod4 = Spec|0x68,
};

/* XKB's real modifiers */
enum {
	Mshift = 1<<0,
	Mcontrol = 1<<2,
	Mmod1 = 1<<3,
	Mmod4 = 1<<6,
	Mmod5 = 1<<7,
};

static uint32_t tab[Nlayers][Nscan];

/* Linux's codes of the keys with the escape prefix (its atkbd's table) */
static int esccode[Nscan] = {
	[0x10] = 165,	/* previous song */
	[0x19] = 163,	/* next song */
	[0x1c] = 96,	/* keypad enter */
	[0x1d] = 97,	/* right control */
	[0x20] = 113,	/* mute */
	[0x22] = 164,	/* play/pause */
	[0x2e] = 114,	/* volume down */
	[0x30] = 115,	/* volume up */
	[0x35] = 98,	/* keypad slash */
	[0x37] = 99,	/* print */
	[0x38] = 100,	/* right alt */
	[0x46] = 119,	/* pause (break) */
	[0x47] = 102,	/* home */
	[0x48] = 103,	/* up */
	[0x49] = 104,	/* page up */
	[0x4b] = 105,	/* left */
	[0x4d] = 106,	/* right */
	[0x4f] = 107,	/* end */
	[0x50] = 108,	/* down */
	[0x51] = 109,	/* page down */
	[0x52] = 110,	/* insert */
	[0x53] = 111,	/* delete */
	[0x5b] = 125,	/* left meta */
	[0x5c] = 126,	/* right meta */
	[0x5d] = 127,	/* compose */
};

/* a key: its Linux code, and its runes by level */
typedef struct Key Key;
struct Key {
	uint32_t	r[4];	/* none, shift, altgr, shift altgr */
};
static Key key[256];

/* runes of k messages to codes */
typedef struct Map Map;
struct Map {
	uint32_t	r;
	int	code;
};
static Map map[512];
static int nmap;

/* a key without the prefix: its Linux code (set 1 is evdev below 0x59) */
static int
plaincode(int sc)
{
	if(sc > 0 && sc < 0x59)
		return sc;
	if(sc == 0x79)
		return 92;	/* henkan */
	if(sc == 0x7b)
		return 94;	/* muhenkan */
	return 0;
}

static void
addmap(uint32_t r, int code)
{
	int i;

	if(r == 0 || code == 0)
		return;
	for(i = 0; i < nmap; i++)
		if(map[i].r == r)
			return;
	if(nmap < (int)(sizeof map/sizeof map[0])){
		map[nmap].r = r;
		map[nmap].code = code;
		nmap++;
	}
}

/* the Linux code of a key of a k message, -1 if none */
int
runekey(uint32_t r)
{
	int i;

	for(i = 0; i < nmap; i++)
		if(map[i].r == r)
			return map[i].code;
	return -1;
}

static uint32_t
modof(uint32_t r)
{
	switch(r){
	case Kshift:	return Mshift;
	case Kctl:	return Mcontrol;
	case Kalt:	return Mmod1;
	case Kmod4:	return Mmod4;
	case Kaltgr:	return Mmod5;
	}
	return 0;
}

/* the modifiers of the keys held (Linux codes) */
uint32_t
keymods(uint32_t *codes, int n)
{
	uint32_t m;
	int i;

	m = 0;
	for(i = 0; i < n; i++)
		if(codes[i] < 256)
			m |= modof(key[codes[i]].r[0]);
	return m;
}

static char *ascii[] = {
	"space", "exclam", "quotedbl", "numbersign", "dollar", "percent", "ampersand", "apostrophe",
	"parenleft", "parenright", "asterisk", "plus", "comma", "minus", "period", "slash",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"colon", "semicolon", "less", "equal", "greater", "question", "at",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"bracketleft", "backslash", "bracketright", "asciicircum", "underscore", "grave",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
	"braceleft", "bar", "braceright", "asciitilde",
};

/* the name of a rune's keysym, nil if it has none */
static char*
keysym(uint32_t r, char *buf, int n)
{
	switch(r){
	case 0:		return NULL;
	case Kesc:	return "Escape";
	case '\b':	return "BackSpace";
	case '\t':	return "Tab";
	case '\n':	return "Return";
	case Kdel:	return "Delete";
	case Kctl:	return "Control_L";
	case Kshift:	return "Shift_L";
	case Kalt:	return "Alt_L";
	case Kaltgr:	return "ISO_Level3_Shift";
	case Kmod4:	return "Super_L";
	case Kcaps:	return "Caps_Lock";
	case Knum:	return "Num_Lock";
	case Kscroll:	return "Scroll_Lock";
	case Kbreak:	return "Pause";
	case Kprint:	return "Print";
	case Khome:	return "Home";
	case Kend:	return "End";
	case Kup:	return "Up";
	case Kdown:	return "Down";
	case Kleft:	return "Left";
	case Kright:	return "Right";
	case Kpgup:	return "Prior";
	case Kpgdown:	return "Next";
	case Kins:	return "Insert";
	case Kmute:	return "XF86AudioMute";
	case Kvoldn:	return "XF86AudioLowerVolume";
	case Kvolup:	return "XF86AudioRaiseVolume";
	case Kpause:	return "XF86AudioPlay";
	case Ksbwd:	return "XF86AudioPrev";
	case Ksfwd:	return "XF86AudioNext";
	case Kbrtdn:	return "XF86MonBrightnessDown";
	case Kbrtup:	return "XF86MonBrightnessUp";
	}
	if(r > KF && r <= (KF|12)){
		snprintf(buf, n, "F%d", (int)(r - KF));
		return buf;
	}
	if(r >= 0x20 && r < 0x7f)
		return ascii[r - 0x20];
	if(r < 0xa0 || (r >= KF && r < 0xF900) || r > 0x10ffff)
		return NULL;	/* control characters, Plan 9's own keys */
	snprintf(buf, n, "U%04X", r);
	return buf;
}

typedef struct Text Text;
struct Text {
	char	*s;
	int	n, max;
};

static void
put(Text *t, char *fmt, ...)
{
	va_list a;
	int n;

	for(;;){
		va_start(a, fmt);
		n = vsnprintf(t->s + t->n, t->max - t->n, fmt, a);
		va_end(a);
		if(n < t->max - t->n)
			break;
		t->max = 2*t->max + n + 1;
		if((t->s = realloc(t->s, t->max)) == NULL){
			fprintf(stderr, "wlsys: keymap: out of memory\n");
			exit(1);
		}
	}
	t->n += n;
}

static int
readkbmap(char *path)
{
	FILE *f;
	char name[32];
	int t, sc, r, n;

	if((f = fopen(path, "r")) == NULL)
		return -1;
	memset(tab, 0, sizeof tab);	/* read again: nothing of the tables before */
	n = 0;
	while(fscanf(f, "%31s %d %d", name, &sc, &r) == 3){
		for(t = 0; t < Nlayers; t++)
			if(strcmp(name, layertab[t]) == 0)
				break;
		if(t == Nlayers || sc < 0 || sc >= Nscan)
			continue;
		tab[t][sc] = r;
		n++;
	}
	fclose(f);
	return n;
}

/*
 * the keymap of kbdfs' tables in path (/dev/kbmap): its text, with the
 * null byte Wayland's keymap event counts, in *text; its length or -1
 */
int
mkkeymap(char *path, char **text)
{
	static char *mods[] = { "Shift", "Control", "Mod1", "Mod4", "Mod5" };
	static uint32_t modr[] = { Kshift, Kctl, Kalt, Kmod4, Kaltgr };
	char b[4][32], *s[4], *type;
	Text t;
	int sc, c, i, l, nl, first;

	if(readkbmap(path) <= 0)
		return -1;
	memset(key, 0, sizeof key);
	nmap = 0;
	/* keys without the prefix first: their runes are the k messages' */
	for(sc = 0; sc < Nscan; sc++){
		if((c = plaincode(sc)) == 0 || tab[Lnone][sc] == 0)
			continue;
		key[c].r[0] = tab[Lnone][sc];
		key[c].r[1] = tab[Lshift][sc];
		key[c].r[2] = tab[Laltgr][sc];
		key[c].r[3] = tab[Lshiftaltgr][sc];
	}
	for(sc = 0; sc < Nscan; sc++)
		if((c = esccode[sc]) != 0 && tab[Lesc1][sc] != 0)
			key[c].r[0] = tab[Lesc1][sc];
	/*
	 * a rune of several keys is the first one's: the main block, then
	 * those with the prefix (arrows before henkan's Kup and Kdown)
	 */
	for(sc = 0; sc < 0x59; sc++)
		addmap(tab[Lnone][sc], plaincode(sc));
	for(sc = 0; sc < Nscan; sc++)
		addmap(tab[Lesc1][sc], esccode[sc]);
	for(sc = 0x59; sc < Nscan; sc++)
		addmap(tab[Lnone][sc], plaincode(sc));

	t.max = 32*1024;
	t.n = 0;
	if((t.s = malloc(t.max)) == NULL)
		return -1;
	put(&t, "xkb_keymap {\n");
	put(&t, "xkb_keycodes \"wlsys\" {\n\tminimum = 8;\n\tmaximum = 255;\n");
	for(c = 1; c < 248; c++)
		if(key[c].r[0] != 0)
			put(&t, "\t<K%d> = %d;\n", c, c+8);
	put(&t, "};\n");
	put(&t, "xkb_types \"wlsys\" {\n"
		"\ttype \"ONE_LEVEL\" {\n\t\tmodifiers = none;\n\t\tlevel_name[Level1] = \"Any\";\n\t};\n"
		"\ttype \"TWO_LEVEL\" {\n\t\tmodifiers = Shift;\n\t\tmap[Shift] = Level2;\n"
		"\t\tlevel_name[Level1] = \"Base\";\n\t\tlevel_name[Level2] = \"Shift\";\n\t};\n"
		"\ttype \"FOUR_LEVEL\" {\n\t\tmodifiers = Shift+Mod5;\n\t\tmap[Shift] = Level2;\n"
		"\t\tmap[Mod5] = Level3;\n\t\tmap[Shift+Mod5] = Level4;\n"
		"\t\tlevel_name[Level1] = \"Base\";\n\t\tlevel_name[Level2] = \"Shift\";\n"
		"\t\tlevel_name[Level3] = \"AltGr\";\n\t\tlevel_name[Level4] = \"Shift AltGr\";\n\t};\n"
		"};\n");
	put(&t, "xkb_compat \"wlsys\" {\n};\n");
	put(&t, "xkb_symbols \"wlsys\" {\n");
	for(c = 1; c < 248; c++){
		if(key[c].r[0] == 0)
			continue;
		for(l = 0; l < 4; l++)
			s[l] = keysym(key[c].r[l], b[l], sizeof b[l]);
		if(s[0] == NULL)
			continue;
		if(s[2] != NULL || s[3] != NULL){
			type = "FOUR_LEVEL";
			nl = 4;
		}else if(s[1] != NULL && strcmp(s[0], s[1]) != 0){
			type = "TWO_LEVEL";
			nl = 2;
		}else{
			type = "ONE_LEVEL";
			nl = 1;
		}
		put(&t, "\tkey <K%d> { type = \"%s\", [", c, type);
		for(l = 0; l < nl; l++)
			put(&t, "%s %s", l ? "," : "", s[l] != NULL ? s[l] : "NoSymbol");
		put(&t, " ] };\n");
	}
	for(i = 0; i < 5; i++){
		first = 1;
		for(c = 1; c < 248; c++)
			if(key[c].r[0] == modr[i]){
				if(first)
					put(&t, "\tmodifier_map %s { <K%d>", mods[i], c);
				else
					put(&t, ", <K%d>", c);
				first = 0;
			}
		if(!first)
			put(&t, " };\n");
	}
	put(&t, "};\n};\n");
	*text = t.s;
	return t.n + 1;
}
