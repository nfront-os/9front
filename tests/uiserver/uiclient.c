/*
 * SPDX-License-Identifier: MIT
 *
 * uiclient: a client of ui(4) in C, through libui (ui(2)): ui in a new
 * window (uinew), a scene of a label, a button and a check box; each click
 * of the button counts in the label, the check box's state is said; ends
 * after two clicks or when ui does.  A line each event, "ready" once the
 * scene is there (with where ui's files are), "count n" at the end.
 * An alarm ends it after 60 s: a test does not hang on it.
 */
#include <u.h>
#include <libc.h>
#include <ui.h>

void
main(void)
{
	Ui *u;
	Uievent e;
	int count;

	alarm(60*1000);
	if((u = uinew("uiclient", "/mnt/uic")) == nil)
		sysfatal("uinew: %r");
	if(uictl(u, "new l label\nset l text count 0\nnew b button\nset b text add one\n"
		"new c checkbox\nset c text check me\nset c checked 0\n") < 0)
		sysfatal("uictl: %r");
	if(uiset(u, "c", "text", " check me ") < 0)	/* quoted: blanks at its ends */
		sysfatal("uiset: %r");
	print("ready %s %s\n", u->srv, u->mtpt);
	count = 0;
	while(uievent(u, &e) >= 0){
		print("event %s %s %s\n", e.kind, e.id ? e.id : "", e.arg);
		if(strcmp(e.kind, "close") == 0)
			break;
		if(strcmp(e.kind, "click") == 0 && strcmp(e.id, "b") == 0){
			count++;
			if(uictl(u, "set l text count %d", count) < 0)
				sysfatal("uictl: %r");
			if(count == 2)
				break;
		}
	}
	print("count %d\n", count);
	uiclose(u);
	exits(nil);
}
