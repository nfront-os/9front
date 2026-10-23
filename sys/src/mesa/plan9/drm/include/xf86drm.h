/*
 * SPDX-License-Identifier: MIT
 *
 * <xf86drm.h> on 9front: the part of libdrm's interface Mesa's
 * drivers made for DRM use, implemented over gpu(4) (../drm9.c).
 * Declarations only, with libdrm's names; the uAPI structures and
 * ioctl numbers are Mesa's (include/drm-uapi).
 */
#ifndef _XF86DRM_H_
#define _XF86DRM_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <drm-uapi/drm.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DRM_NODE_PRIMARY	0
#define DRM_NODE_CONTROL	1
#define DRM_NODE_RENDER		2
#define DRM_NODE_MAX		3

#define DRM_BUS_PCI		0
#define DRM_BUS_USB		1
#define DRM_BUS_PLATFORM	2
#define DRM_BUS_HOST1X		3

#define DRM_DEVICE_GET_PCI_REVISION	(1 << 0)

#define DRM_PLATFORM_DEVICE_NAME_LEN	512
#define DRM_HOST1X_DEVICE_NAME_LEN	512

typedef unsigned int drm_magic_t;

typedef struct _drmPciBusInfo {
	uint16_t	domain;
	uint8_t	bus;
	uint8_t	dev;
	uint8_t	func;
} drmPciBusInfo, *drmPciBusInfoPtr;

typedef struct _drmPciDeviceInfo {
	uint16_t	vendor_id;
	uint16_t	device_id;
	uint16_t	subvendor_id;
	uint16_t	subdevice_id;
	uint8_t	revision_id;
} drmPciDeviceInfo, *drmPciDeviceInfoPtr;

typedef struct _drmUsbBusInfo {
	uint8_t	bus;
	uint8_t	dev;
} drmUsbBusInfo, *drmUsbBusInfoPtr;

typedef struct _drmUsbDeviceInfo {
	uint16_t	vendor;
	uint16_t	product;
} drmUsbDeviceInfo, *drmUsbDeviceInfoPtr;

typedef struct _drmPlatformBusInfo {
	char	fullname[DRM_PLATFORM_DEVICE_NAME_LEN];
} drmPlatformBusInfo, *drmPlatformBusInfoPtr;

typedef struct _drmPlatformDeviceInfo {
	char	**compatible;
} drmPlatformDeviceInfo, *drmPlatformDeviceInfoPtr;

typedef struct _drmHost1xBusInfo {
	char	fullname[DRM_HOST1X_DEVICE_NAME_LEN];
} drmHost1xBusInfo, *drmHost1xBusInfoPtr;

typedef struct _drmHost1xDeviceInfo {
	char	**compatible;
} drmHost1xDeviceInfo, *drmHost1xDeviceInfoPtr;

typedef struct _drmDevice {
	char	**nodes;		/* DRM_NODE_MAX of them */
	int	available_nodes;	/* 1<<DRM_NODE_* */
	int	bustype;
	union {
		drmPciBusInfoPtr	pci;
		drmUsbBusInfoPtr	usb;
		drmPlatformBusInfoPtr	platform;
		drmHost1xBusInfoPtr	host1x;
	} businfo;
	union {
		drmPciDeviceInfoPtr	pci;
		drmUsbDeviceInfoPtr	usb;
		drmPlatformDeviceInfoPtr	platform;
		drmHost1xDeviceInfoPtr	host1x;
	} deviceinfo;
} drmDevice, *drmDevicePtr;

typedef struct _drmVersion {
	int	version_major;
	int	version_minor;
	int	version_patchlevel;
	int	name_len;
	char	*name;
	int	date_len;
	char	*date;
	int	desc_len;
	char	*desc;
} drmVersion, *drmVersionPtr;

extern int	drmIoctl(int, unsigned long, void*);
extern drmVersionPtr	drmGetVersion(int);
extern void	drmFreeVersion(drmVersionPtr);
extern int	drmGetCap(int, uint64_t, uint64_t*);
extern int	drmSetClientCap(int, uint64_t, uint64_t);
extern int	drmGetMagic(int, drm_magic_t*);
extern int	drmAuthMagic(int, drm_magic_t);
extern int	drmSetMaster(int);
extern int	drmDropMaster(int);
extern int	drmIsMaster(int);

extern int	drmGetDevice2(int, uint32_t, drmDevicePtr*);
extern int	drmGetDevice(int, drmDevicePtr*);
extern int	drmGetDevices2(uint32_t, drmDevicePtr[], int);
extern int	drmGetDevices(drmDevicePtr[], int);
extern int	drmGetDeviceFromDevId(dev_t, uint32_t, drmDevicePtr*);
extern void	drmFreeDevice(drmDevicePtr*);
extern void	drmFreeDevices(drmDevicePtr[], int);
extern int	drmDevicesEqual(drmDevicePtr, drmDevicePtr);
extern int	drmGetNodeTypeFromFd(int);
extern char	*drmGetDeviceNameFromFd2(int);
extern char	*drmGetPrimaryDeviceNameFromFd(int);
extern char	*drmGetRenderDeviceNameFromFd(int);

extern int	drmPrimeHandleToFD(int, uint32_t, uint32_t, int*);
extern int	drmPrimeFDToHandle(int, int, uint32_t*);
extern int	drmCloseBufferHandle(int, uint32_t);

extern int	drmSyncobjCreate(int, uint32_t, uint32_t*);
extern int	drmSyncobjDestroy(int, uint32_t);
extern int	drmSyncobjHandleToFD(int, uint32_t, int*);
extern int	drmSyncobjFDToHandle(int, int, uint32_t*);
extern int	drmSyncobjImportSyncFile(int, uint32_t, int);
extern int	drmSyncobjExportSyncFile(int, uint32_t, int*);
extern int	drmSyncobjWait(int, uint32_t*, unsigned, int64_t, unsigned, uint32_t*);
extern int	drmSyncobjReset(int, const uint32_t*, uint32_t);
extern int	drmSyncobjSignal(int, const uint32_t*, uint32_t);
extern int	drmSyncobjTimelineSignal(int, const uint32_t*, uint64_t*, uint32_t);
extern int	drmSyncobjTimelineWait(int, uint32_t*, uint64_t*, unsigned, int64_t, unsigned, uint32_t*);
extern int	drmSyncobjQuery(int, uint32_t*, uint64_t*, uint32_t);
extern int	drmSyncobjQuery2(int, uint32_t*, uint64_t*, uint32_t, uint32_t);
extern int	drmSyncobjTransfer(int, uint32_t, uint64_t, uint32_t, uint64_t, uint32_t);

#ifdef __cplusplus
}
#endif
#endif
