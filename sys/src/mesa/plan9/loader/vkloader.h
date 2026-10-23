/*
 * SPDX-License-Identifier: MIT
 *
 * the static Vulkan loader of 9front (see genloader.py)
 */
#define VK_USE_PLATFORM_PLAN9_9FRONT
#include <vulkan/vulkan.h>
#include <vulkan/vk_icd.h>
#include "vkcmds.h"	/* VKL_NCMD, generated */

/* the dispatch table of an instance (and its physical devices) or a device */
typedef struct Vkl Vkl;
struct Vkl {
	PFN_vkVoidFunction fn[VKL_NCMD];
	struct Vkl *inst;	/* for a device: its instance's table */
	struct Vklicd *icd;	/* the driver */
};

/* a driver (an ICD) linked in: its vk_icdGetInstanceProcAddr, renamed <name>_icd... */
typedef struct Vklicd Vklicd;
struct Vklicd {
	char	*name;
	PFN_vkGetInstanceProcAddr	gipa;
	int	negotiated;
};

/* the table pointer the loader puts in the first word of every dispatchable object */
#define VKL_DISPATCH(h)	(*(Vkl**)(h))

extern const char *const vkl_names[VKL_NCMD];
extern const unsigned char vkl_level[VKL_NCMD];
extern const unsigned short vkl_sorted[VKL_NCMD];
extern PFN_vkVoidFunction const vkl_tramp[VKL_NCMD];

PFN_vkVoidFunction vkl_global(int);
