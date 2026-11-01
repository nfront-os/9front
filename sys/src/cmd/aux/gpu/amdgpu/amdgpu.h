/*
 * SPDX-License-Identifier: MIT
 *
 * aux/amdgpu: what the Plan 9 side calls on the Linux side
 * (linux/amdgpu.c); plain C types only, both sides include it.
 */
/* before amdgpu_start: DRM's debug categories, printk's level */
void	amdgpu_options(unsigned long drmdebug, int loglevel);
int	amdgpu_start(void);	/* the scheduler's and amdgpu's module_init: probes the device; 0 or -errno */
char	*amdgpu_status(void);	/* amdgpu_freestr */
void	amdgpu_freestr(char *s);
