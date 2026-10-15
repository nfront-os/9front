/* https://en.wikipedia.org/wiki/Code_page_437 */
#include "tagspriv.h"

static Rune rh[129] =
#ifdef __GNUC__	/* GCC port: kencc's L"" is Rune[], gcc's wchar_t (int)[]: U"" (char32_t, unsigned) there */
	U"ΔÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧"
	U"ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜"
	U"╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌"
	U"▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠"
	U"⌡÷≈°∙·√ⁿ²■ ";
#else
	L"ΔÇüéâäàåçêëèïîìÄÅÉæÆôöòûùÿÖÜ¢£¥₧"
	L"ƒáíóúñÑªº¿⌐¬½¼¡«»░▒▓│┤╡╢╖╕╣║╗╝╜"
	L"╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌"
	L"▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠"
	L"⌡÷≈°∙·√ⁿ²■ ";
#endif

int
cp437toutf8(uchar *o, int osz, const uchar *s, int sz)
{
	char c[UTFmax];
	int i, n;
	Rune r;

	for(i = 0; i < sz && osz > 1 && s[i] != 0; i++){
		if(s[i] < 127){
			*o++ = s[i];
			osz--;
			continue;
		}
		r = rh[s[i] - 127];
		if((n = runetochar(c, &r)) >= osz)
			break;
		memmove(o, c, n);
		o += n;
		osz -= n;
	}

	*o = 0;
	return i;
}
