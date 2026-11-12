/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: OpenBSD kernel API, sys/device.h
 */
#ifndef _OBSD_SYS_DEVICE_H
#define _OBSD_SYS_DEVICE_H

#include <sys/types.h>
#include <sys/queue.h>

struct cfdriver {
	void	**cd_devs;
	const char	*cd_name;
	int	cd_class;
	int	cd_mode;
	int	cd_ndevs;
};

struct cfdata {
	struct cfattach	*cf_attach;
	struct cfdriver	*cf_driver;
};

/*
 * struct device is Linux's here (linux/device.h): the Linux code reaches
 * into it; what OpenBSD's code asks of a device goes through that
 */
struct device;

/* firmware(9): a file of /lib/firmware, malloc'd (kpi/obsd/firmware.c); 0 or an errno */
int	loadfirmware(const char *, unsigned char **, size_t *);

#define DV_DULL		0
#define DVF_ACTIVE	0x0001

#endif
