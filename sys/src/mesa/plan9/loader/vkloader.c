/*
 * SPDX-License-Identifier: MIT
 *
 * The static Vulkan loader of 9front: libvulkan.a is this loader and
 * the drivers (ICDs: Mesa's venus, hasvk) linked together, as 9front
 * has no shared libraries.  Programs call vkCreateInstance, vkCmdDraw
 * and the rest as anywhere else.
 *
 * An instance is one driver's: the first, in the order of icds[], that
 * finds a physical device (hasvk on Intel's gen 7-8, venus on
 * virtio-gpu), or the one $VK_DRIVER names.  Each driver's entry points
 * are renamed (vk_icdGetInstanceProcAddr to
 * intel_hasvk_icdGetInstanceProcAddr, ...: Mesa's patch for 9front);
 * the ones not built are weak, nil.
 *
 * As the Khronos loader, it keeps a dispatch table in the first word of
 * every dispatchable object (VK_LOADER_DATA, which the driver leaves
 * alone): the instance's for the instance and its physical devices,
 * the device's for the device, its queues and command buffers.  The
 * trampolines (vkcmds.c, from genloader.py) call through it; the
 * commands that make dispatchable objects are here.  No layers.
 *
 * The functions are vkl_CreateInstance etc., the vk names weak aliases
 * of them: a program that keeps its own pointers under the vk names
 * (vkcube, vulkaninfo; they would dlopen libvulkan.so.1 elsewhere)
 * starts from vkl_GetInstanceProcAddr instead.
 */
#include <stdlib.h>
#include <string.h>
#include "vkloader.h"

#define nelem(a)	(sizeof(a)/sizeof((a)[0]))

VKAPI_ATTR void VKAPI_CALL vkl_DestroyInstance(VkInstance, const VkAllocationCallbacks*);

/*
 * the drivers' entry points (vk_icd.h, interface version 5+): each its
 * vk_icdGetInstanceProcAddr, renamed (Mesa's patch for 9front); the
 * negotiation is Mesa's Vulkan runtime's, which they share
 */
#define ICD(n) \
	extern VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL n##_icdGetInstanceProcAddr(VkInstance, const char*) __attribute__((weak));
ICD(intel_hasvk)
ICD(venus)
extern VKAPI_ATTR VkResult VKAPI_CALL vk_icdNegotiateLoaderICDInterfaceVersion(uint32_t*);

static Vklicd icds[] = {
	{ "intel_hasvk", intel_hasvk_icdGetInstanceProcAddr },
	{ "venus", venus_icdGetInstanceProcAddr },
};

/* the drivers to try, in order: built in, and $VK_DRIVER's if set */
static int
usable(Vklicd *d)
{
	char *s;

	if(d->gipa == NULL)
		return 0;
	s = getenv("VK_DRIVER");
	return s == NULL || *s == 0 || strcmp(s, d->name) == 0;
}

static PFN_vkVoidFunction
icd(Vklicd *d, VkInstance inst, const char *name)
{
	uint32_t v;

	if(!d->negotiated){
		v = 7;
		vk_icdNegotiateLoaderICDInterfaceVersion(&v);
		d->negotiated = 1;
	}
	return d->gipa(inst, name);
}

/* commands without an instance: the first usable driver's */
static Vklicd*
firsticd(void)
{
	int i;

	for(i = 0; i < nelem(icds); i++)
		if(usable(&icds[i]))
			return &icds[i];
	return NULL;
}

PFN_vkVoidFunction
vkl_global(int i)
{
	Vklicd *d;

	if((d = firsticd()) == NULL)
		return NULL;
	return icd(d, NULL, vkl_names[i]);
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
vkl_GetInstanceProcAddr(VkInstance inst, const char *name)
{
	Vklicd *d;
	int i;

	if(strcmp(name, "vkGetInstanceProcAddr") == 0)
		return (PFN_vkVoidFunction)vkl_GetInstanceProcAddr;
	if(inst == NULL){
		i = lookup(name);
		if(i < 0 || vkl_level[i] != 0)
			return NULL;
		d = firsticd();
		if(d == NULL || icd(d, NULL, name) == NULL)
			return NULL;
		return vkl_tramp[i];	/* vkl_global's */
	}
	return wrap(name, icd(VKL_DISPATCH(inst)->icd, inst, name));
}

VKAPI_ATTR PFN_vkVoidFunction VKAPI_CALL
vkl_GetDeviceProcAddr(VkDevice dev, const char *name)
{
	PFN_vkGetDeviceProcAddr gdpa;

	gdpa = (PFN_vkGetDeviceProcAddr)VKL_DISPATCH(dev)->fn[VKL_vkGetDeviceProcAddr];
	return wrap(name, gdpa(dev, name));
}

/* no layers: they are the loader's, as with Khronos' loader */
VKAPI_ATTR VkResult VKAPI_CALL
vkl_EnumerateInstanceLayerProperties(uint32_t *n, VkLayerProperties *p)
{
	(void)p;
	*n = 0;
	return VK_SUCCESS;
}

/* the instance extensions of all the drivers, as with Khronos' loader */
VKAPI_ATTR VkResult VKAPI_CALL
vkl_EnumerateInstanceExtensionProperties(const char *layer, uint32_t *n, VkExtensionProperties *p)
{
	PFN_vkEnumerateInstanceExtensionProperties f;
	VkExtensionProperties *all, *e;
	uint32_t na, ne, i, j;
	VkResult r;
	int k;

	if(layer != NULL)
		return VK_ERROR_LAYER_NOT_PRESENT;
	all = NULL;
	na = 0;
	for(k = 0; k < nelem(icds); k++){
		if(!usable(&icds[k]))
			continue;
		f = (PFN_vkEnumerateInstanceExtensionProperties)icd(&icds[k], NULL, "vkEnumerateInstanceExtensionProperties");
		if(f == NULL || f(NULL, &ne, NULL) != VK_SUCCESS || ne == 0)
			continue;
		e = calloc(ne, sizeof *e);
		if(e == NULL){
			free(all);
			return VK_ERROR_OUT_OF_HOST_MEMORY;
		}
		if(f(NULL, &ne, e) < 0){
			free(e);
			continue;
		}
		for(i = 0; i < ne; i++){
			for(j = 0; j < na; j++)
				if(strcmp(all[j].extensionName, e[i].extensionName) == 0)
					break;
			if(j < na){
				if(e[i].specVersion > all[j].specVersion)
					all[j].specVersion = e[i].specVersion;
				continue;
			}
			if((all = realloc(all, (na+1) * sizeof *all)) == NULL){
				free(e);
				return VK_ERROR_OUT_OF_HOST_MEMORY;
			}
			all[na++] = e[i];
		}
		free(e);
	}
	r = VK_SUCCESS;
	if(p == NULL)
		*n = na;
	else{
		if(*n < na)
			r = VK_INCOMPLETE;
		else
			*n = na;
		memcpy(p, all, *n * sizeof *p);
	}
	free(all);
	return r;
}

/* an instance of driver d, its table; nil if that fails */
static VkResult
mkinstance(Vklicd *d, const VkInstanceCreateInfo *ci, const VkAllocationCallbacks *a, VkInstance *pinst)
{
	PFN_vkCreateInstance create;
	PFN_vkDestroyInstance destroy;
	VkResult r;
	Vkl *t;
	int i;

	create = (PFN_vkCreateInstance)icd(d, NULL, "vkCreateInstance");
	if(create == NULL)
		return VK_ERROR_INCOMPATIBLE_DRIVER;
	t = calloc(1, sizeof *t);
	if(t == NULL)
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	r = create(ci, a, pinst);
	if(r == VK_ERROR_EXTENSION_NOT_PRESENT)
		r = VK_ERROR_INCOMPATIBLE_DRIVER;
	if(r != VK_SUCCESS){
		free(t);
		return r;
	}
	t->icd = d;
	for(i = 0; i < VKL_NCMD; i++)
		if(vkl_level[i] != 0)
			t->fn[i] = icd(d, *pinst, vkl_names[i]);
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

/*
 * an instance of driver d with the extensions of ci it has, as
 * Khronos' loader passes each driver (the program asked for those of
 * all of them: vkEnumerateInstanceExtensionProperties)
 */
static VkResult
icdinstance(Vklicd *d, const VkInstanceCreateInfo *ci, const VkAllocationCallbacks *a, VkInstance *pinst)
{
	PFN_vkEnumerateInstanceExtensionProperties f;
	VkInstanceCreateInfo nci;
	VkExtensionProperties *e;
	const char **names;
	uint32_t ne, i, j, n;
	VkResult r;

	f = (PFN_vkEnumerateInstanceExtensionProperties)icd(d, NULL, "vkEnumerateInstanceExtensionProperties");
	if(f == NULL || ci->enabledExtensionCount == 0 || f(NULL, &ne, NULL) != VK_SUCCESS)
		return mkinstance(d, ci, a, pinst);
	e = calloc(ne+1, sizeof *e);
	names = calloc(ci->enabledExtensionCount, sizeof *names);
	if(e == NULL || names == NULL || f(NULL, &ne, e) < 0){
		free(e);
		free(names);
		return VK_ERROR_OUT_OF_HOST_MEMORY;
	}
	n = 0;
	for(i = 0; i < ci->enabledExtensionCount; i++)
		for(j = 0; j < ne; j++)
			if(strcmp(ci->ppEnabledExtensionNames[i], e[j].extensionName) == 0){
				names[n++] = ci->ppEnabledExtensionNames[i];
				break;
			}
	nci = *ci;
	nci.enabledExtensionCount = n;
	nci.ppEnabledExtensionNames = names;
	r = mkinstance(d, &nci, a, pinst);
	free(e);
	free(names);
	return r;
}

/*
 * the instance of the first driver that finds a physical device; if
 * none does, of the first that makes an instance (with no devices)
 */
VKAPI_ATTR VkResult VKAPI_CALL
vkl_CreateInstance(const VkInstanceCreateInfo *ci, const VkAllocationCallbacks *a, VkInstance *pinst)
{
	PFN_vkEnumeratePhysicalDevices enumerate;
	VkInstance inst, first;
	VkResult r, r0;
	uint32_t n;
	int i;

	first = VK_NULL_HANDLE;
	r0 = VK_ERROR_INCOMPATIBLE_DRIVER;
	for(i = 0; i < nelem(icds); i++){
		if(!usable(&icds[i]))
			continue;
		r = icdinstance(&icds[i], ci, a, &inst);
		if(r != VK_SUCCESS){
			if(r0 == VK_ERROR_INCOMPATIBLE_DRIVER)
				r0 = r;
			continue;
		}
		enumerate = (PFN_vkEnumeratePhysicalDevices)VKL_DISPATCH(inst)->fn[VKL_vkEnumeratePhysicalDevices];
		n = 0;
		if(enumerate(inst, &n, NULL) == VK_SUCCESS && n > 0){
			if(first != VK_NULL_HANDLE)
				vkl_DestroyInstance(first, a);
			*pinst = inst;
			return VK_SUCCESS;
		}
		if(first == VK_NULL_HANDLE)
			first = inst;
		else
			vkl_DestroyInstance(inst, a);
	}
	if(first == VK_NULL_HANDLE)
		return r0;
	*pinst = first;
	return VK_SUCCESS;
}

VKAPI_ATTR void VKAPI_CALL
vkl_DestroyInstance(VkInstance inst, const VkAllocationCallbacks *a)
{
	Vkl *t;

	if(inst == VK_NULL_HANDLE)
		return;
	t = VKL_DISPATCH(inst);
	((PFN_vkDestroyInstance)t->fn[VKL_vkDestroyInstance])(inst, a);
	free(t);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkl_EnumeratePhysicalDevices(VkInstance inst, uint32_t *n, VkPhysicalDevice *pd)
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
vkl_EnumeratePhysicalDeviceGroups(VkInstance inst, uint32_t *n, VkPhysicalDeviceGroupProperties *g)
{
	return groups(VKL_vkEnumeratePhysicalDeviceGroups, inst, n, g);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkl_EnumeratePhysicalDeviceGroupsKHR(VkInstance inst, uint32_t *n, VkPhysicalDeviceGroupProperties *g)
{
	return groups(VKL_vkEnumeratePhysicalDeviceGroupsKHR, inst, n, g);
}

VKAPI_ATTR VkResult VKAPI_CALL
vkl_CreateDevice(VkPhysicalDevice pd, const VkDeviceCreateInfo *ci, const VkAllocationCallbacks *a, VkDevice *pdev)
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
vkl_DestroyDevice(VkDevice dev, const VkAllocationCallbacks *a)
{
	Vkl *t;

	if(dev == VK_NULL_HANDLE)
		return;
	t = VKL_DISPATCH(dev);
	((PFN_vkDestroyDevice)t->fn[VKL_vkDestroyDevice])(dev, a);
	free(t);
}

VKAPI_ATTR void VKAPI_CALL
vkl_GetDeviceQueue(VkDevice dev, uint32_t family, uint32_t index, VkQueue *q)
{
	Vkl *t;

	t = VKL_DISPATCH(dev);
	((PFN_vkGetDeviceQueue)t->fn[VKL_vkGetDeviceQueue])(dev, family, index, q);
	if(*q != VK_NULL_HANDLE)
		VKL_DISPATCH(*q) = t;
}

VKAPI_ATTR void VKAPI_CALL
vkl_GetDeviceQueue2(VkDevice dev, const VkDeviceQueueInfo2 *qi, VkQueue *q)
{
	Vkl *t;

	t = VKL_DISPATCH(dev);
	((PFN_vkGetDeviceQueue2)t->fn[VKL_vkGetDeviceQueue2])(dev, qi, q);
	if(*q != VK_NULL_HANDLE)
		VKL_DISPATCH(*q) = t;
}

VKAPI_ATTR VkResult VKAPI_CALL
vkl_AllocateCommandBuffers(VkDevice dev, const VkCommandBufferAllocateInfo *ai, VkCommandBuffer *cb)
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

VKL_HAND_ALIASES
