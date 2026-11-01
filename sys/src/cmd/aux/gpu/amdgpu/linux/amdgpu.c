// SPDX-License-Identifier: MIT
/*
 * aux/amdgpu: the Linux side, where Linux's names are seen: the module
 * inits (the DRM scheduler's, amdgpu's) and what the device came up as.
 */
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/reboot.h>
#include <drm/drm_print.h>
#include <drm/drm_connector.h>
#include "amdgpu.h"
#include "amdgpu_dpm.h"
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

/* a sensor of the power management (the SMU's), else -1 */
static long long
sensor(struct amdgpu_device *a, enum amd_pp_sensors which)
{
	u32 v = 0, size = sizeof(v);

	if (amdgpu_dpm_read_sensor(a, which, &v, &size) != 0)
		return -1;
	return v;
}

/* millidegrees as "d.d C", or "-" */
static int
temp(char *p, char *e, const char *name, long long mdeg, int crit)
{
	if (mdeg < 0)
		return scnprintf(p, e - p, " %s -", name);
	return scnprintf(p, e - p, " %s %lld.%lld C (crit %d C)", name, mdeg / 1000, (mdeg % 1000) / 100, crit / 1000);
}

/*
 * the temperatures, the power, the clocks, the load and the fan, as
 * amdgpu's hwmon and amdgpu_pm_info show them
 */
static char *
power(char *p, char *e, struct amdgpu_device *a)
{
	long long t, w, sclk, mclk, load;
	u32 rpm;

	p += scnprintf(p, e - p, "temp");
	p += temp(p, e, "edge", sensor(a, AMDGPU_PP_SENSOR_EDGE_TEMP), a->pm.dpm.thermal.max_temp);
	p += temp(p, e, "hotspot", sensor(a, AMDGPU_PP_SENSOR_HOTSPOT_TEMP), a->pm.dpm.thermal.max_hotspot_crit_temp);
	p += temp(p, e, "mem", sensor(a, AMDGPU_PP_SENSOR_MEM_TEMP), a->pm.dpm.thermal.max_mem_crit_temp);
	p += scnprintf(p, e - p, "\n");
	w = sensor(a, AMDGPU_PP_SENSOR_GPU_AVG_POWER);	/* watts, 8 bits of fraction */
	sclk = sensor(a, AMDGPU_PP_SENSOR_GFX_SCLK);	/* 10 kHz */
	mclk = sensor(a, AMDGPU_PP_SENSOR_GFX_MCLK);
	load = sensor(a, AMDGPU_PP_SENSOR_GPU_LOAD);
	t = amdgpu_dpm_get_fan_speed_rpm(a, &rpm) == 0 ? rpm : -1;
	p += scnprintf(p, e - p, "power %lld.%02lld W, gfx %lld MHz, mem %lld MHz, load %lld%%, fan %lld rpm\n",
	    w < 0 ? -1 : w >> 8, w < 0 ? 0 : ((w & 0xff) * 100) >> 8,
	    sclk < 0 ? -1 : sclk / 100, mclk < 0 ? -1 : mclk / 100, load, t);
	return p;
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
	if (a->pm.dpm_enabled)
		p = power(p, e, a);
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

/* -X: what amdgpu does at a critical temperature (smu_v13_0_irq_process: CTF), to try it */
void
amdgpu_test_poweroff(void)
{
	pr_emerg("amdgpu: -X: as at a critical temperature: orderly_poweroff\n");
	orderly_poweroff(true);
}

void
amdgpu_freestr(char *s)
{
	kfree(s);
}
