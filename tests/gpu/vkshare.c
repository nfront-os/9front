/*
 * SPDX-License-Identifier: MIT
 *
 * vkshare: a GPU buffer shared between two processes (gpu(4)'s export and
 * import, ../9front-ui stage 2): "vkshare export file" allocates exportable
 * device memory (dma-buf), fills it on the GPU with a pattern, gets its fd
 * (vkGetMemoryFdKHR) and writes the fd's export token to file
 * (plan9/lib/dmabuf.c), then waits for file.done; "vkshare import file" makes
 * an fd of the token again, imports the memory (VkImportMemoryFdInfoKHR),
 * copies it on the GPU into a buffer of its own and checks the pattern.
 * An ordinary Vulkan program but for the two plan9_dmabuf calls.
 *
 * -s: the GPUs' work in order through a timeline semaphore the exporter
 * shares as well (VK_KHR_external_semaphore_fd, opaque: a syncobj): the
 * importer submits its copy waiting for value 1 first, and only then
 * (file.ready, half a second later) does the exporter fill and signal 1;
 * the copy has the pattern, and the importer's wait took that long.
 *
 * usage: vkshare [-s] export|import file
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <vulkan/vulkan.h>

extern int plan9_dmabuf_fd(const char *token);
extern int plan9_dmabuf_token(int fd, char *buf, int n);

enum { Size = 1<<20 };
#define PATTERN(i) (0x5a000000u | (unsigned)(i))

#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); exit(1); } while(0)
#define CHECK(call, what) do { VkResult r_ = (call); if(r_ != VK_SUCCESS) FAIL("%s: VkResult %d", what, r_); } while(0)

static VkInstance inst;
static VkPhysicalDevice pd;
static VkDevice dev;
static VkQueue q;
static uint32_t qf;
static VkCommandPool pool;
static char *role;
static int sem;	/* -s */

static double
now(void)
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC, &t);
	return t.tv_sec + t.tv_nsec / 1e9;
}

static void
waitfile(char *name)
{
	struct stat st;
	int i;

	for(i = 0; i < 600 && stat(name, &st) < 0; i++)
		usleep(50000);
	if(i == 600)
		FAIL("no %s", name);
}

/* a timeline semaphore, exportable as an fd (opaque) */
static VkSemaphore
timeline(int exportable)
{
	VkSemaphore sm;
	VkExportSemaphoreCreateInfo exp = {
		.sType = VK_STRUCTURE_TYPE_EXPORT_SEMAPHORE_CREATE_INFO,
		.handleTypes = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
	};
	VkSemaphoreTypeCreateInfo ty = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
		.pNext = exportable ? &exp : NULL,
		.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
		.initialValue = 0,
	};
	VkSemaphoreCreateInfo sci = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = &ty,
	};
	CHECK(vkCreateSemaphore(dev, &sci, NULL, &sm), "vkCreateSemaphore (timeline)");
	return sm;
}

static void
setup(void)
{
	uint32_t n;
	VkQueueFamilyProperties qfp[16];
	float prio = 1.0f;
	const char *dext[] = { VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME, VK_EXT_EXTERNAL_MEMORY_DMA_BUF_EXTENSION_NAME,
		VK_KHR_EXTERNAL_SEMAPHORE_FD_EXTENSION_NAME };
	VkPhysicalDeviceVulkan12Features f12 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
		.timelineSemaphore = VK_TRUE,
	};
	VkApplicationInfo app = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "vkshare",
		.apiVersion = VK_API_VERSION_1_2,
	};
	VkInstanceCreateInfo ici = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &app,
	};
	CHECK(vkCreateInstance(&ici, NULL, &inst), "vkCreateInstance");
	n = 1;
	vkEnumeratePhysicalDevices(inst, &n, &pd);
	if(n == 0)
		FAIL("no physical device");
	n = 16;
	vkGetPhysicalDeviceQueueFamilyProperties(pd, &n, qfp);
	for(qf = 0; qf < n; qf++)
		if(qfp[qf].queueFlags & VK_QUEUE_TRANSFER_BIT || qfp[qf].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			break;
	if(qf == n)
		FAIL("no queue family");
	VkDeviceQueueCreateInfo qci = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
		.queueFamilyIndex = qf,
		.queueCount = 1,
		.pQueuePriorities = &prio,
	};
	VkDeviceCreateInfo dci = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &qci,
		.pNext = sem ? &f12 : NULL,
		.enabledExtensionCount = sem ? 3 : 2,
		.ppEnabledExtensionNames = dext,
	};
	CHECK(vkCreateDevice(pd, &dci, NULL, &dev), "vkCreateDevice (external memory fd, dma-buf)");
	vkGetDeviceQueue(dev, qf, 0, &q);
	VkCommandPoolCreateInfo pci = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = qf,
	};
	CHECK(vkCreateCommandPool(dev, &pci, NULL, &pool), "vkCreateCommandPool");
}

static uint32_t
memtype(uint32_t bits, VkMemoryPropertyFlags want)
{
	VkPhysicalDeviceMemoryProperties mp;
	uint32_t i;

	vkGetPhysicalDeviceMemoryProperties(pd, &mp);
	for(i = 0; i < mp.memoryTypeCount; i++)
		if((bits & 1u<<i) && (mp.memoryTypes[i].propertyFlags & want) == want)
			return i;
	FAIL("no memory type for bits %#x flags %#x", bits, want);
	return 0;
}

static VkBuffer
buffer(int external)
{
	VkBuffer b;
	VkExternalMemoryBufferCreateInfo ext = {
		.sType = VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_BUFFER_CREATE_INFO,
		.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
	};
	VkBufferCreateInfo bci = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.pNext = external ? &ext : NULL,
		.size = Size,
		.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
	};
	CHECK(vkCreateBuffer(dev, &bci, NULL, &b), "vkCreateBuffer");
	return b;
}

/* record into a one-time command buffer, submit, wait */
static VkCommandBuffer
begin(void)
{
	VkCommandBuffer cb;
	VkCommandBufferAllocateInfo cai = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = pool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1,
	};
	VkCommandBufferBeginInfo bi = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
	};
	CHECK(vkAllocateCommandBuffers(dev, &cai, &cb), "vkAllocateCommandBuffers");
	CHECK(vkBeginCommandBuffer(cb, &bi), "vkBeginCommandBuffer");
	return cb;
}

/* cb submitted, waiting for wait's value 1 and signalling sig's 1 (each if not null); its fence */
static VkFence
submit(VkCommandBuffer cb, VkSemaphore wait, VkSemaphore sig)
{
	VkFence f;
	uint64_t one = 1;
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	VkFenceCreateInfo fci = { .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
	VkTimelineSemaphoreSubmitInfo ts = {
		.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
		.waitSemaphoreValueCount = wait ? 1 : 0,
		.pWaitSemaphoreValues = &one,
		.signalSemaphoreValueCount = sig ? 1 : 0,
		.pSignalSemaphoreValues = &one,
	};
	VkSubmitInfo si = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = wait || sig ? &ts : NULL,
		.waitSemaphoreCount = wait ? 1 : 0,
		.pWaitSemaphores = &wait,
		.pWaitDstStageMask = &stage,
		.commandBufferCount = 1,
		.pCommandBuffers = &cb,
		.signalSemaphoreCount = sig ? 1 : 0,
		.pSignalSemaphores = &sig,
	};
	CHECK(vkEndCommandBuffer(cb), "vkEndCommandBuffer");
	CHECK(vkCreateFence(dev, &fci, NULL, &f), "vkCreateFence");
	CHECK(vkQueueSubmit(q, 1, &si, f), "vkQueueSubmit");
	return f;
}

static void
finish(VkFence f)
{
	CHECK(vkWaitForFences(dev, 1, &f, VK_TRUE, 10000000000ULL), "vkWaitForFences");
	vkDestroyFence(dev, f, NULL);
}

static void
run(VkCommandBuffer cb)
{
	finish(submit(cb, VK_NULL_HANDLE, VK_NULL_HANDLE));
}

static void
export(char *file)
{
	VkBuffer b;
	VkDeviceMemory m;
	VkMemoryRequirements mr;
	char token[64], done[256];
	FILE *o;
	int fd, i;

	b = buffer(1);
	vkGetBufferMemoryRequirements(dev, b, &mr);
	VkExportMemoryAllocateInfo exp = {
		.sType = VK_STRUCTURE_TYPE_EXPORT_MEMORY_ALLOCATE_INFO,
		.handleTypes = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
	};
	VkMemoryAllocateInfo mai = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = &exp,
		.allocationSize = mr.size,
		.memoryTypeIndex = memtype(mr.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
	};
	CHECK(vkAllocateMemory(dev, &mai, NULL, &m), "vkAllocateMemory (exportable)");
	CHECK(vkBindBufferMemory(dev, b, m, 0), "vkBindBufferMemory");

	/* the pattern, word by word: a fill per 4 KB page with its number */
	VkCommandBuffer cb = begin();
	for(i = 0; i < Size/4096; i++)
		vkCmdFillBuffer(cb, b, i*4096, 4096, PATTERN(i));
	if(!sem){
		run(cb);
		printf("ok %s: %d KB filled on the GPU\n", role, Size/1024);
	}

	VkMemoryGetFdInfoKHR gfi = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_GET_FD_INFO_KHR,
		.memory = m,
		.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
	};
	PFN_vkGetMemoryFdKHR getfd = (PFN_vkGetMemoryFdKHR)vkGetDeviceProcAddr(dev, "vkGetMemoryFdKHR");
	if(getfd == NULL)
		FAIL("no vkGetMemoryFdKHR");
	CHECK(getfd(dev, &gfi, &fd), "vkGetMemoryFdKHR");
	if(plan9_dmabuf_token(fd, token, sizeof token) < 0)
		FAIL("fd %d is no export token", fd);
	printf("ok %s: dma-buf fd %d, token %.8s...\n", role, fd, token);
	/* -s: the semaphore's token after the memory's */
	VkSemaphore sm = VK_NULL_HANDLE;
	char stoken[64] = "";
	int sfd = -1;
	if(sem){
		sm = timeline(1);
		VkSemaphoreGetFdInfoKHR sgi = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_GET_FD_INFO_KHR,
			.semaphore = sm,
			.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
		};
		PFN_vkGetSemaphoreFdKHR getsfd = (PFN_vkGetSemaphoreFdKHR)vkGetDeviceProcAddr(dev, "vkGetSemaphoreFdKHR");
		if(getsfd == NULL)
			FAIL("no vkGetSemaphoreFdKHR");
		CHECK(getsfd(dev, &sgi, &sfd), "vkGetSemaphoreFdKHR");
		if(plan9_dmabuf_token(sfd, stoken, sizeof stoken) < 0)
			FAIL("semaphore fd %d is no export token", sfd);
		printf("ok %s: semaphore fd %d, token %.8s...\n", role, sfd, stoken);
	}
	if((o = fopen(file, "w")) == NULL)
		FAIL("%s: cannot create", file);
	fprintf(o, "%s %s\n", token, stoken);
	fclose(o);

	/* -s: the importer's copy waits on the GPU; fill, then signal it */
	if(sem){
		snprintf(done, sizeof done, "%s.ready", file);
		waitfile(done);
		usleep(500000);
		finish(submit(cb, VK_NULL_HANDLE, sm));
		printf("ok %s: %d KB filled on the GPU, semaphore signalled\n", role, Size/1024);
	}

	/* the memory stays while the importer reads it */
	snprintf(done, sizeof done, "%s.done", file);
	waitfile(done);
	if(sem){
		close(sfd);
		vkDestroySemaphore(dev, sm, NULL);
	}
	close(fd);
	vkDestroyBuffer(dev, b, NULL);
	vkFreeMemory(dev, m, NULL);
	printf("vkshare export: all passed\n");
}

static void
import(char *file)
{
	VkBuffer b, hb;
	VkDeviceMemory m, hm;
	VkMemoryRequirements mr, hmr;
	char line[160], token[64], stoken[64], done[256];
	unsigned *w;
	FILE *in;
	int fd, i, bad;

	for(i = 0; i < 600; i++){
		if((in = fopen(file, "r")) != NULL){
			if(fgets(line, sizeof line, in) != NULL && strchr(line, '\n') != NULL)
				break;
			fclose(in);
		}
		usleep(50000);
	}
	if(i == 600)
		FAIL("no token in %s", file);
	fclose(in);
	stoken[0] = 0;
	if(sscanf(line, "%63s %63s", token, stoken) < 1)
		FAIL("no token in %s", file);
	if(sem && stoken[0] == 0)
		FAIL("no semaphore token in %s", file);
	if((fd = plan9_dmabuf_fd(token)) < 0)
		FAIL("no fd for token");

	b = buffer(1);
	vkGetBufferMemoryRequirements(dev, b, &mr);
	VkMemoryFdPropertiesKHR fp = { .sType = VK_STRUCTURE_TYPE_MEMORY_FD_PROPERTIES_KHR };
	PFN_vkGetMemoryFdPropertiesKHR fdprops = (PFN_vkGetMemoryFdPropertiesKHR)vkGetDeviceProcAddr(dev, "vkGetMemoryFdPropertiesKHR");
	if(fdprops == NULL)
		FAIL("no vkGetMemoryFdPropertiesKHR");
	CHECK(fdprops(dev, VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT, fd, &fp), "vkGetMemoryFdPropertiesKHR");
	VkImportMemoryFdInfoKHR imp = {
		.sType = VK_STRUCTURE_TYPE_IMPORT_MEMORY_FD_INFO_KHR,
		.handleType = VK_EXTERNAL_MEMORY_HANDLE_TYPE_DMA_BUF_BIT_EXT,
		.fd = fd,
	};
	VkMemoryAllocateInfo mai = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.pNext = &imp,
		.allocationSize = mr.size,
		.memoryTypeIndex = memtype(mr.memoryTypeBits & fp.memoryTypeBits, 0),
	};
	CHECK(vkAllocateMemory(dev, &mai, NULL, &m), "vkAllocateMemory (import)");
	CHECK(vkBindBufferMemory(dev, b, m, 0), "vkBindBufferMemory");
	printf("ok %s: imported the token's memory\n", role);

	/* a host visible buffer of our own, copied into on the GPU */
	hb = buffer(0);
	vkGetBufferMemoryRequirements(dev, hb, &hmr);
	VkMemoryAllocateInfo hai = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = hmr.size,
		.memoryTypeIndex = memtype(hmr.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
	};
	CHECK(vkAllocateMemory(dev, &hai, NULL, &hm), "vkAllocateMemory (host)");
	CHECK(vkBindBufferMemory(dev, hb, hm, 0), "vkBindBufferMemory (host)");
	VkCommandBuffer cb = begin();
	VkBufferCopy cp = { 0, 0, Size };
	vkCmdCopyBuffer(cb, b, hb, 1, &cp);
	if(!sem)
		run(cb);
	else{
		/* the exporter's semaphore here; the copy waits for its 1 */
		VkSemaphore sm = timeline(0);
		int sfd = plan9_dmabuf_fd(stoken);
		if(sfd < 0)
			FAIL("no fd for the semaphore's token");
		VkImportSemaphoreFdInfoKHR isi = {
			.sType = VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_FD_INFO_KHR,
			.semaphore = sm,
			.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_FD_BIT,
			.fd = sfd,
		};
		PFN_vkImportSemaphoreFdKHR impsfd = (PFN_vkImportSemaphoreFdKHR)vkGetDeviceProcAddr(dev, "vkImportSemaphoreFdKHR");
		if(impsfd == NULL)
			FAIL("no vkImportSemaphoreFdKHR");
		CHECK(impsfd(dev, &isi), "vkImportSemaphoreFdKHR");
		printf("ok %s: imported the semaphore\n", role);
		VkFence f = submit(cb, sm, VK_NULL_HANDLE);
		double t0 = now();
		snprintf(done, sizeof done, "%s.ready", file);
		close(creat(done, 0666));
		finish(f);
		double t = now() - t0;
		if(t < 0.4)
			FAIL("the copy did not wait for the exporter: done after %.3f s", t);
		printf("ok %s: the copy waited %.2f s on the GPU for the exporter's signal\n", role, t);
		vkDestroySemaphore(dev, sm, NULL);
	}
	CHECK(vkMapMemory(dev, hm, 0, Size, 0, (void**)&w), "vkMapMemory");
	bad = 0;
	for(i = 0; i < Size/4; i++)
		if(w[i] != PATTERN(i*4/4096)){
			if(bad++ == 0)
				printf("word %d: %#x, want %#x\n", i, w[i], PATTERN(i*4/4096));
		}
	vkUnmapMemory(dev, hm);
	if(bad)
		FAIL("%d words not the exporter's", bad);
	printf("ok %s: all %d words are the exporter's\n", role, Size/4);
	snprintf(done, sizeof done, "%s.done", file);
	close(creat(done, 0666));
	vkDestroyBuffer(dev, b, NULL);
	vkFreeMemory(dev, m, NULL);
	printf("vkshare import: all passed\n");
}

int
main(int argc, char **argv)
{
	if(argc > 1 && strcmp(argv[1], "-s") == 0){
		sem = 1;
		argc--;
		argv++;
	}
	if(argc != 3 || (strcmp(argv[1], "export") != 0 && strcmp(argv[1], "import") != 0)){
		fprintf(stderr, "usage: vkshare [-s] export|import file\n");
		return 1;
	}
	role = argv[1];
	setbuf(stdout, NULL);
	setup();
	if(strcmp(role, "export") == 0)
		export(argv[2]);
	else
		import(argv[2]);
	return 0;
}
