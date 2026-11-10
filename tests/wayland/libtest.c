/*
 * SPDX-License-Identifier: MIT
 *
 * libtest: the libraries for APE that foot needs, on 9front (scripts/ape/
 * vendor.sh: sys/src/freetype, pixman, expat, xkbcommon, fontconfig): freetype renders
 * an A of /lib/font/ttf/FiraSans-Regular.ttf at 32 pixels; pixman blends
 * half transparent red over blue; expat parses a little document;
 * xkbcommon compiles the keymap wlsys made of kbdfs' tables (wlclient
 * saved it as /tmp/keymap.xkb): x's key is x, with shift X; fontconfig
 * reads /lib/fontconfig/fonts.conf, finds the fonts of /lib/font/ttf and
 * matches sans-serif to Fira Sans' file.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <pixman.h>
#include <expat.h>
#include <xkbcommon/xkbcommon.h>
#include <fontconfig/fontconfig.h>

static int nfail;

static void
check(const char *what, int ok, const char *fmt, ...)
{
	va_list a;

	if(ok){
		printf("ok %s\n", what);
		return;
	}
	printf("FAIL %s", what);
	if(fmt != NULL){
		printf(": ");
		va_start(a, fmt);
		vprintf(fmt, a);
		va_end(a);
	}
	printf("\n");
	nfail++;
}

static void
testfreetype(void)
{
	FT_Library ft;
	FT_Face face;
	FT_Bitmap *b;
	int e, x, y, ink;

	e = FT_Init_FreeType(&ft);
	check("freetype: FT_Init_FreeType", e == 0, "error %d", e);
	if(e != 0)
		return;
	e = FT_New_Face(ft, "/lib/font/ttf/FiraSans-Regular.ttf", 0, &face);
	check("freetype: FiraSans-Regular.ttf opened", e == 0, "error %d", e);
	if(e != 0)
		return;
	check("freetype: its family", face->family_name != NULL && strcmp(face->family_name, "Fira Sans") == 0,
		"%s", face->family_name ? face->family_name : "none");
	FT_Set_Pixel_Sizes(face, 0, 32);
	e = FT_Load_Char(face, 'A', FT_LOAD_RENDER);
	check("freetype: A rendered at 32 pixels", e == 0, "error %d", e);
	if(e != 0)
		return;
	b = &face->glyph->bitmap;
	ink = 0;
	for(y = 0; y < (int)b->rows; y++)
		for(x = 0; x < (int)b->width; x++)
			if(b->buffer[y*b->pitch + x] > 128)
				ink++;
	check("freetype: the A's bitmap, 15 to 32 rows, inked", b->rows >= 15 && b->rows <= 32 && b->width >= 10 && ink > 50,
		"%dx%d, %d dark pixels", b->width, b->rows, ink);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

static void
testpixman(void)
{
	uint32_t px[4*4];
	pixman_image_t *dst, *src;
	pixman_color_t red = {0x8000, 0, 0, 0x8000};	/* premultiplied: red at half alpha */
	uint32_t v;
	int i;

	for(i = 0; i < 16; i++)
		px[i] = 0xff0000ff;	/* opaque blue */
	dst = pixman_image_create_bits(PIXMAN_a8r8g8b8, 4, 4, px, 16);
	src = pixman_image_create_solid_fill(&red);
	check("pixman: images made", dst != NULL && src != NULL, NULL);
	if(dst == NULL || src == NULL)
		return;
	pixman_image_composite32(PIXMAN_OP_OVER, src, NULL, dst, 0, 0, 0, 0, 1, 1, 2, 2);
	v = px[1*4 + 1];
	check("pixman: half red over blue at 1 1", (v>>16 & 0xff) >= 0x7e && (v>>16 & 0xff) <= 0x81 && (v & 0xff) >= 0x7e && (v & 0xff) <= 0x81,
		"%08x", v);
	check("pixman: outside it blue still", px[0] == 0xff0000ff && px[15] == 0xff0000ff, "%08x %08x", px[0], px[15]);
	pixman_image_unref(src);
	pixman_image_unref(dst);
}

static int elements;
static char attr[32], text[32];

static void XMLCALL
start(void *data, const XML_Char *name, const XML_Char **atts)
{
	(void)data;
	elements++;
	if(strcmp(name, "font") == 0 && atts[0] != NULL && strcmp(atts[0], "family") == 0)
		snprintf(attr, sizeof attr, "%s", atts[1]);
}

static void XMLCALL
end(void *data, const XML_Char *name)
{
	(void)data; (void)name;
}

static void XMLCALL
chars(void *data, const XML_Char *s, int n)
{
	(void)data;
	if(n > 0 && strlen(text) + n < sizeof text)
		strncat(text, s, n);
}

static void
testexpat(void)
{
	static char doc[] = "<?xml version=\"1.0\"?><fontconfig><font family=\"monospace\">Go Mono</font></fontconfig>";
	XML_Parser p;
	int ok;

	p = XML_ParserCreate(NULL);
	check("expat: a parser", p != NULL, NULL);
	if(p == NULL)
		return;
	XML_SetElementHandler(p, start, end);
	XML_SetCharacterDataHandler(p, chars);
	ok = XML_Parse(p, doc, strlen(doc), 1) == XML_STATUS_OK;
	check("expat: the document parsed", ok, "%s", XML_ErrorString(XML_GetErrorCode(p)));
	check("expat: two elements, the attribute, the text", elements == 2 && strcmp(attr, "monospace") == 0 && strcmp(text, "Go Mono") == 0,
		"%d, %s, %s", elements, attr, text);
	XML_ParserFree(p);
}

static void
testxkb(void)
{
	struct xkb_context *ctx;
	struct xkb_keymap *km;
	struct xkb_state *st;
	xkb_keysym_t sym;
	FILE *f;
	char *s, name[32];
	long n;

	ctx = xkb_context_new(XKB_CONTEXT_NO_DEFAULT_INCLUDES | XKB_CONTEXT_NO_ENVIRONMENT_NAMES);
	check("xkbcommon: a context", ctx != NULL, NULL);
	if(ctx == NULL)
		return;
	if((f = fopen("/tmp/keymap.xkb", "r")) == NULL){
		check("xkbcommon: /tmp/keymap.xkb (wlclient's)", 0, "not there");
		return;
	}
	fseek(f, 0, SEEK_END);
	n = ftell(f);
	fseek(f, 0, SEEK_SET);
	s = calloc(1, n + 1);
	fread(s, 1, n, f);
	fclose(f);
	km = xkb_keymap_new_from_string(ctx, s, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
	check("xkbcommon: wlsys' keymap compiled", km != NULL, NULL);
	if(km == NULL)
		return;
	st = xkb_state_new(km);
	sym = xkb_state_key_get_one_sym(st, 45 + 8);
	xkb_keysym_get_name(sym, name, sizeof name);
	check("xkbcommon: x's key (45) is x", sym == XKB_KEY_x, "%s", name);
	xkb_state_update_mask(st, 1 << xkb_keymap_mod_get_index(km, XKB_MOD_NAME_SHIFT), 0, 0, 0, 0, 0);
	sym = xkb_state_key_get_one_sym(st, 45 + 8);
	xkb_keysym_get_name(sym, name, sizeof name);
	check("xkbcommon: with shift X", sym == XKB_KEY_X, "%s", name);
	xkb_state_unref(st);
	xkb_keymap_unref(km);
	xkb_context_unref(ctx);
	free(s);
}

static void
testfontconfig(void)
{
	FcConfig *cfg;
	FcFontSet *fs;
	FcPattern *p, *m;
	FcResult r;
	FcChar8 *file, *fam;

	cfg = FcInitLoadConfigAndFonts();
	check("fontconfig: /lib/fontconfig/fonts.conf loaded", cfg != NULL, NULL);
	if(cfg == NULL)
		return;
	fs = FcConfigGetFonts(cfg, FcSetSystem);
	check("fontconfig: the fonts of /lib/font/ttf", fs != NULL && fs->nfont >= 1, "%d", fs ? fs->nfont : -1);
	p = FcNameParse((const FcChar8*)"sans-serif:style=Regular");
	FcConfigSubstitute(cfg, p, FcMatchPattern);
	FcDefaultSubstitute(p);
	m = FcFontMatch(cfg, p, &r);
	check("fontconfig: sans-serif matched", m != NULL && r == FcResultMatch, "%d", r);
	if(m != NULL){
		file = fam = NULL;
		FcPatternGetString(m, FC_FILE, 0, &file);
		FcPatternGetString(m, FC_FAMILY, 0, &fam);
		check("fontconfig: to FiraSans-Regular.ttf, Fira Sans",
			file != NULL && strcmp((char*)file, "/lib/font/ttf/FiraSans-Regular.ttf") == 0 &&
			fam != NULL && strcmp((char*)fam, "Fira Sans") == 0,
			"%s, %s", file ? (char*)file : "none", fam ? (char*)fam : "none");
		FcPatternDestroy(m);
	}
	FcPatternDestroy(p);
	FcConfigDestroy(cfg);
}

int
main(void)
{
	testfreetype();
	testpixman();
	testexpat();
	testxkb();
	testfontconfig();
	printf(nfail ? "libtest: %d failed\n" : "libtest: all passed\n", nfail);
	return nfail != 0;
}
