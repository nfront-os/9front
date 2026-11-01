// SPDX-License-Identifier: MIT
/*
 * aux/amdgpu: the Linux side, where Linux's names are seen: the module
 * inits (the DRM scheduler's, amdgpu's) and what the device came up as.
 */
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <drm/drm_print.h>
#include <drm/drm_connector.h>
#include "amdgpu.h"
#include "../amdgpu.h"

int	kpi_init_drm_sched_fence_slab_init(void);	/* drivers/gpu/drm/scheduler/sched_fence.c */
int	kpi_init_amdgpu_init(void);	/* drivers/gpu/drm/amd/amdgpu/amdgpu_drv.c */
struct pci_dev	*kpi_pci_gpu(const char *gdir);
extern int kpi_loglevel;
extern const char *amdgpu_asic_name[];	/* amdgpu_device.c */

void
amdgpu_options(unsigned long drmdebug, int loglevel)
{
	__drm_debug = drmdebug;
	kpi_loglevel = loglevel;
}

int
amdgpu_start(void)
{
	int r;

	if ((r = kpi_init_drm_sched_fence_slab_init()) < 0)
		return r;
	return kpi_init_amdgpu_init();
}

static struct amdgpu_device *
adev(void)
{
	struct pci_dev *p = kpi_pci_gpu(NULL);
	struct drm_device *d;

	if (!p || !(d = pci_get_drvdata(p)))
		return NULL;
	return drm_to_adev(d);
}

/* the chip, its memory and the blocks that came up */
char *
amdgpu_status(void)
{
	struct amdgpu_device *a = adev();
	char *s, *e, *p;
	int i;

	s = kmalloc(4096, GFP_KERNEL);
	if (!s)
		return NULL;
	p = s;
	e = s + 4096;
	if (!a) {
		snprintf(s, e - s, "device none\n");
		return s;
	}
	p += scnprintf(p, e - p, "device amd %04x/%04x (Linux 6.6's amdgpu on the LinuxKPI)\n",
	    a->pdev->vendor, a->pdev->device);
	p += scnprintf(p, e - p, "asic %s family %u rev %u external rev %u\n",
	    amdgpu_asic_name[a->asic_type], a->family, a->rev_id, a->external_rev_id);
	p += scnprintf(p, e - p, "vram %llu MB visible %llu MB gtt %llu MB\n",
	    a->gmc.real_vram_size >> 20, a->gmc.visible_vram_size >> 20, a->gmc.gart_size >> 20);
	p += scnprintf(p, e - p, "accel %s\n", a->accel_working ? "working" : "not working");
	for (i = 0; i < a->num_ip_blocks; i++)
		p += scnprintf(p, e - p, "ip %s valid %d sw %d hw %d late %d\n",
		    a->ip_blocks[i].version->funcs->name,
		    a->ip_blocks[i].status.valid, a->ip_blocks[i].status.sw,
		    a->ip_blocks[i].status.hw, a->ip_blocks[i].status.late_initialized);
	return s;
}

/* drm_sysfs: a connector's state changed (the file server will tell its clients) */
void
kpi_drm_hotplug(struct drm_device *dev, struct drm_connector *connector)
{
	if (connector)
		pr_info("amdgpu: hotplug %s\n", connector->name);
	else
		pr_info("amdgpu: hotplug\n");
}

void
amdgpu_freestr(char *s)
{
	kfree(s);
}
