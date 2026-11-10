/*
 * 9front-ng: <linux/input-event-codes.h> for Wayland clients (foot): the
 * Linux event codes of wl_pointer's buttons and wl_keyboard's keys
 * (wlsys(4) sends them).  Linux's own header is GPL; these are the codes
 * the clients built here use, written down by hand.  Add others as a
 * client needs them.
 */
#ifndef _LINUX_INPUT_EVENT_CODES_H
#define _LINUX_INPUT_EVENT_CODES_H

#define KEY_ESC		1
#define KEY_2		3
#define KEY_BACKSPACE	14
#define KEY_TAB		15
#define KEY_Y		21
#define KEY_ENTER	28
#define KEY_A		30
#define KEY_APOSTROPHE	40
#define KEY_SPACE	57
#define KEY_F11		87
#define KEY_LEFT	105

#define BTN_LEFT	0x110
#define BTN_RIGHT	0x111
#define BTN_MIDDLE	0x112
#define BTN_SIDE	0x113
#define BTN_EXTRA	0x114
#define BTN_FORWARD	0x115
#define BTN_BACK	0x116
#define BTN_TASK	0x117

#endif
