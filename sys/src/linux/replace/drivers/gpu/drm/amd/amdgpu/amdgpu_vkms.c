/* Public domain. */
/*
 * 9front: OpenBSD's empty virtual display (Linux's amdgpu_vkms.c is GPL),
 * with functions, so that the block can be added (its name is printed):
 * a virtual display (amdgpu.virtual_display, SR-IOV) is not there, the
 * device does not come up with it.
 */

#include "amdgpu.h"

static int
amdgpu_vkms_early_init(struct amdgpu_ip_block *ip_block)	/* 9front: Linux 6.13's form */
{
	struct amdgpu_device *adev = ip_block->adev;

	dev_err(adev->dev, "no virtual display here (amdgpu.virtual_display, SR-IOV)\n");
	return -ENODEV;
}

static const struct amd_ip_funcs amdgpu_vkms_ip_funcs = {
	.name = "amdgpu_vkms",
	.early_init = amdgpu_vkms_early_init,
};

const struct amdgpu_ip_block_version amdgpu_vkms_ip_block = {
	.type = AMD_IP_BLOCK_TYPE_DCE,
	.major = 1,
	.minor = 0,
	.rev = 0,
	.funcs = &amdgpu_vkms_ip_funcs,
};
