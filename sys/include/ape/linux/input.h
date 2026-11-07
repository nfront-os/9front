/*
 * 9front-ng: <linux/input.h> for Wayland clients (../9front-ui PLAN §12):
 * wl_pointer's buttons and wl_keyboard's keys are Linux's event codes
 * (wlsys(4) sends them).  Linux's own header is GPL; these are the codes
 * the clients built here use, written down by hand.  Add others as a
 * client needs them.
 */
#ifndef _LINUX_INPUT_H
#define _LINUX_INPUT_H

#define KEY_ESC		1
#define KEY_F11		87

#define BTN_LEFT	0x110
#define BTN_RIGHT	0x111
#define BTN_MIDDLE	0x112

#endif
