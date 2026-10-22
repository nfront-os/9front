/*
 * SPDX-License-Identifier: MIT
 *
 * The static Vulkan loader of 9front: libvulkan.a is this loader and
 * one driver (an ICD, such as Mesa's venus) linked together, as 9front
 * has no shared libraries.  Programs call vkCreateInstance, vkCmdDraw
 * and the rest as anywhere else.
 *
 * As the Khronos loader, it keeps a dispatch table in the first word of
 * every dispatchable object (VK_LOADER_DATA, which the driver leaves
 * alone): the instance's for the instance and its physical devices,
 * the device's for the device, its queues and command buffers.  The
 * trampolines (vkcmds.c, from genloader.py) call through it; the
 * commands that make dispatchable objects are here.  No layers.
 */
#include <stdlib.h>
#include <string.h>
#include "vkloader.h"

/* the driver's entry points (vk_icd.h, interface version 5+) */
extern VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL vk_icdGetInstanceProcAddr(VkInstance, const char*);
extern VKAPI_ATTR VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t*);

static PFN_vkVoidFunction
icd(VkInstance inst, const char *name)
{
	static int negotiated;
	uint32_t v;

	if(!negotiated){
		v = 7;
		vk_icdNegotiateLoaderICDInterfaceVersion(&v);
		negotiated = 1;
	}
	return vk_icdGetInstanceProcAddr(inst, name);
}

PFN_vkVoidFunction
vkl_global(int i)
{
	return icd(NULL, vkl_names[i]);
}

static int
lookup(const char *name)
{
	int lo, hi, m, c;

	lo = 0;
	hi = VKL_NCMD;
	while(lo < hi){
		m = (lo+hi)/2;
		c = strcmp(name, vkl_names[vkl_sorted[m]]);
		if(c == 0)
			return vkl_sorted[m];
		if(c < 0)
			hi = m;
		else
			lo = m+1;
	}
	return -1;
}

/* the loader's own function for a command the driver has, else the driver's */
static PFN_vkVoidFunction
wrap(const char *name, PFN_vkVoidFunction p)
{
	int i;

	if(p == NULL)
		return NULL;
	i = lookup(name);
	return i < 0 ? p : vkl_tramp[i];
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetInstanceProcAddr(VkInstance inst, const char *name)
{
	int i;

	if(strcmp(name, "vkGetInstanceProcAddr") == 0)
		return (PFN_vkVoidFunction)vkGetInstanceProcAddr;
	if(inst == NULL){
		i = lookup(name);
		if(i < 0 || vkl_level[i] != 0)
			return NULL;
	}
	return wrap(name, icd(inst, name));
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkGetDeviceProcAddr(VkDevice dev, const char *name)
{
	PFN_vkGetDeviceProcAddr gdpa;

	gdpa = (PFN_vkGetDeviceProcAddr)VKL_DISPATCH(dev)->fn[VKL_vkGetDeviceProcAddr];
	return wrap(name, gdpa(dev, name));
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateInstance(const VkInstanceCreateInfo *ci, const VkAllocationCallbacks *a, VkInstance *pinst)
{
	PFN_vkDestroyInstance destroy;
	VkResult r;
	Vkl *t;
	int i;

	t = calloc(1, sizeof *t);
	if(t == NULL)
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	r = ((PFN_vkCreateInstance)vkl_global(VKL_vkCreateInstance))(ci, a, pinst);
	if(r != VK_SUCCESS){
		free(t);
		return r;
	}
	for(i = 0; i < VKL_NCMD; i++)
		if(vkl_level[i] != 0)
			t->fn[i] = icd(*pinst, vkl_names[i]);
	if(t->fn[VKL_vkGetDeviceProcAddr] == NULL){
		destroy = (PFN_vkDestroyInstance)t->fn[VKL_vkDestroyInstance];
		if(destroy != NULL)
			destroy(*pinst, a);
		free(t);
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	VKL_DISPATCH(*pinst) = t;
	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyInstance(VkInstance inst, const VkAllocationCallbacks *a)
{
	Vkl *t;

	if(inst == VK_NULL_HANDLE)
		return;
	t = VKL_DISPATCH(inst);
	((PFN_vkDestroyInstance)t->fn[VKL_vkDestroyInstance])(inst, a);
	free(t);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDevices(VkInstance inst, uint32_t *n, VkPhysicalDevice *pd)
{
	Vkl *t;
	VkResult r;
	uint32_t i;

	t = VKL_DISPATCH(inst);
	r = ((PFN_vkEnumeratePhysicalDevices)t->fn[VKL_vkEnumeratePhysicalDevices])(inst, n, pd);
	if(pd != NULL && (r == VK_SUCCESS || r == VK_INCOMPLETE))
		for(i = 0; i < *n; i++)
			VKL_DISPATCH(pd[i]) = t;
	return r;
}

static VkResult
groups(int cmd, VkInstance inst, uint32_t *n, VkPhysicalDeviceGroupProperties *g)
{
	Vkl *t;
	VkResult r;
	uint32_t i, j;

	t = VKL_DISPATCH(inst);
	r = ((PFN_vkEnumeratePhysicalDeviceGroups)t->fn[cmd])(inst, n, g);
	if(g != NULL && (r == VK_SUCCESS || r == VK_INCOMPLETE))
		for(i = 0; i < *n; i++)
			for(j = 0; j < g[i].physicalDeviceCount; j++)
				VKL_DISPATCH(g[i].physicalDevices[j]) = t;
	return r;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDeviceGroups(VkInstance inst, uint32_t *n, VkPhysicalDeviceGroupProperties *g)
{
	return groups(VKL_vkEnumeratePhysicalDeviceGroups, inst, n, g);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkEnumeratePhysicalDeviceGroupsKHR(VkInstance inst, uint32_t *n, VkPhysicalDeviceGroupProperties *g)
{
	return groups(VKL_vkEnumeratePhysicalDeviceGroupsKHR, inst, n, g);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkCreateDevice(VkPhysicalDevice pd, const VkDeviceCreateInfo *ci, const VkAllocationCallbacks *a, VkDevice *pdev)
{
	PFN_vkGetDeviceProcAddr gdpa;
	Vkl *it, *t;
	VkResult r;
	int i;

	it = VKL_DISPATCH(pd);
	t = calloc(1, sizeof *t);
	if(t == NULL)
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	r = ((PFN_vkCreateDevice)it->fn[VKL_vkCreateDevice])(pd, ci, a, pdev);
	if(r != VK_SUCCESS){
		free(t);
		return r;
	}
	gdpa = (PFN_vkGetDeviceProcAddr)it->fn[VKL_vkGetDeviceProcAddr];
	for(i = 0; i < VKL_NCMD; i++)
		if(vkl_level[i] == 2)
			t->fn[i] = gdpa(*pdev, vkl_names[i]);
	t->inst = it;
	VKL_DISPATCH(*pdev) = t;
	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkDestroyDevice(VkDevice dev, const VkAllocationCallbacks *a)
{
	Vkl *t;

	if(dev == VK_NULL_HANDLE)
		return;
	t = VKL_DISPATCH(dev);
	((PFN_vkDestroyDevice)t->fn[VKL_vkDestroyDevice])(dev, a);
	free(t);
}

VKAPI_ATTR void VKAPI_CALL
vkGetDeviceQueue(VkDevice dev, uint32_t family, uint32_t index, VkQueue *q)
{
	Vkl *t;

	t = VKL_DISPATCH(dev);
	((PFN_vkGetDeviceQueue)t->fn[VKL_vkGetDeviceQueue])(dev, family, index, q);
	if(*q != VK_NULL_HANDLE)
		VKL_DISPATCH(*q) = t;
}

VKAPI_ATTR void VKAPI_CALL
vkGetDeviceQueue2(VkDevice dev, const VkDeviceQueueInfo2 *qi, VkQueue *q)
{
	Vkl *t;

	t = VKL_DISPATCH(dev);
	((PFN_vkGetDeviceQueue2)t->fn[VKL_vkGetDeviceQueue2])(dev, qi, q);
	if(*q != VK_NULL_HANDLE)
		VKL_DISPATCH(*q) = t;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkAllocateCommandBuffers(VkDevice dev, const VkCommandBufferAllocateInfo *ai, VkCommandBuffer *cb)
{
	Vkl *t;
	VkResult r;
	uint32_t i;

	t = VKL_DISPATCH(dev);
	r = ((PFN_vkAllocateCommandBuffers)t->fn[VKL_vkAllocateCommandBuffers])(dev, ai, cb);
	if(r == VK_SUCCESS)
		for(i = 0; i < ai->commandBufferCount; i++)
			VKL_DISPATCH(cb[i]) = t;
	return r;
}
