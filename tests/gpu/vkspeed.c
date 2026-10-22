/*
 * SPDX-License-Identifier: MIT
 *
 * vkspeed: how fast the CPU reads and writes each host visible memory
 * type of the Vulkan device (venus: host memory through the hostmem
 * bar), with plain loads, SSE4.1 streaming loads and stores. Without a
 * type, those in the host's RAM (not device local: reading video memory
 * through the bus takes minutes), and at the end the fastest streaming
 * read of a cached type (Mesa's plan9 WSI reads the frames from one).
 *
 * usage: vkspeed [type [MB]]	(default: RAM types, 16 MB)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <smmintrin.h>
#include <vulkan/vulkan.h>

#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); exit(1); } while(0)
#define CHECK(call, what) do { VkResult r_ = (call); if(r_ != VK_SUCCESS) FAIL("%s: VkResult %d", what, r_); } while(0)

static size_t Size = 16<<20;

static double
now(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec + ts.tv_nsec/1e9;
}

__attribute__((target("sse4.1"))) static void
streamread(void *dst, const void *src, size_t n)
{
	__m128i *d = dst, *s = (__m128i*)src;
	size_t i;

	for(i = 0; i < n/16; i += 4){
		__m128i a = _mm_stream_load_si128(s+i), b = _mm_stream_load_si128(s+i+1);
		__m128i c = _mm_stream_load_si128(s+i+2), e = _mm_stream_load_si128(s+i+3);
		_mm_store_si128(d+i, a); _mm_store_si128(d+i+1, b);
		_mm_store_si128(d+i+2, c); _mm_store_si128(d+i+3, e);
	}
}

static void
flags(VkMemoryPropertyFlags f, char *s)
{
	*s = 0;
	if(f & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) strcat(s, "local ");
	if(f & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) strcat(s, "visible ");
	if(f & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) strcat(s, "coherent ");
	if(f & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) strcat(s, "cached ");
}

int
main(int argc, char **argv)
{
	int only = -1;
	VkInstance inst;
	VkPhysicalDevice pd;
	VkDevice dev;
	VkPhysicalDeviceMemoryProperties mp;
	uint32_t n, t;
	char fl[64];
	double t0, mb, best = 0;
	char *tmp;

	if(argc > 1)
		only = atoi(argv[1]);
	if(argc > 2)
		Size = (size_t)atoi(argv[2])<<20;
	mb = Size/1e6;
	setvbuf(stdout, NULL, _IONBF, 0);

	VkApplicationInfo app = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO, .apiVersion = VK_API_VERSION_1_1 };
	VkInstanceCreateInfo ici = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, .pApplicationInfo = &app };
	CHECK(vkCreateInstance(&ici, NULL, &inst), "vkCreateInstance");
	n = 1;
	vkEnumeratePhysicalDevices(inst, &n, &pd);
	if(n == 0)
		FAIL("no device");
	float prio = 1;
	VkDeviceQueueCreateInfo qci = { .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueCount = 1, .pQueuePriorities = &prio };
	VkDeviceCreateInfo dci = { .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1, .pQueueCreateInfos = &qci };
	CHECK(vkCreateDevice(pd, &dci, NULL, &dev), "vkCreateDevice");
	vkGetPhysicalDeviceMemoryProperties(pd, &mp);
	tmp = aligned_alloc(64, Size);
	memset(tmp, 1, Size);
	for(t = 0; t < mp.memoryTypeCount; t++){
		VkMemoryPropertyFlags f = mp.memoryTypes[t].propertyFlags;
		if(!(f & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) || (only >= 0 && t != (uint32_t)only))
			continue;
		if(only < 0 && (f & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
			continue;
		flags(f, fl);
		VkMemoryAllocateInfo mai = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = Size, .memoryTypeIndex = t };
		VkDeviceMemory mem;
		void *p;
		if(vkAllocateMemory(dev, &mai, NULL, &mem) != VK_SUCCESS){
			printf("type %u (%s): no %d MB\n", t, fl, (int)(Size>>20));
			continue;
		}
		CHECK(vkMapMemory(dev, mem, 0, Size, 0, &p), "vkMapMemory");
		printf("type %u (%s): mapped at %p\n", t, fl, p);
		t0 = now();
		memcpy(p, tmp, Size);
		double w = mb/(now()-t0);
		t0 = now();
		memcpy(tmp, p, Size);
		double r = mb/(now()-t0);
		t0 = now();
		streamread(tmp, p, Size);
		double s = mb/(now()-t0);
		printf("type %u heap %u (%s): write %.0f MB/s, read %.0f MB/s, streaming read %.0f MB/s\n",
			t, mp.memoryTypes[t].heapIndex, fl, w, r, s);
		if((f & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) && s > best)
			best = s;
		vkUnmapMemory(dev, mem);
		vkFreeMemory(dev, mem, NULL);
	}
	t0 = now();
	memcpy(tmp+Size/2, tmp, Size/2);
	printf("guest memory: copy %.0f MB/s\n", mb/2/(now()-t0));
	vkDestroyDevice(dev, NULL);
	vkDestroyInstance(inst, NULL);
	printf("vkspeed: cached streaming read %.0f MB/s\n", best);
	return 0;
}
