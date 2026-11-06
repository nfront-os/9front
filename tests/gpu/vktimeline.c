/*
 * SPDX-License-Identifier: MIT
 *
 * vktimeline: Vulkan's timeline semaphores (Vulkan 1.2) on 9front, as wgpu
 * uses them for its fences: a queue submission that signals value 1, waited
 * for on the processor (vkWaitSemaphores); the value read back
 * (vkGetSemaphoreCounterValue); a signal from the processor
 * (vkSignalSemaphore) and a wait for it; a submission that waits for a value
 * the processor signals later. Each wait has a time limit (2 s): a hang
 * is a failure, not a hung test.
 * Prints "ok ..." / "FAIL ..." lines and a summary.
 */
#include <vulkan/vulkan.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int nfail;

#define OK(...) do { printf("ok "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); } while(0)
#define FAIL(...) do { printf("FAIL "); printf(__VA_ARGS__); printf("\n"); fflush(stdout); nfail++; } while(0)

static const uint64_t LIMIT = 2000000000ull;	/* 2 s */

static VkDevice dev;
static VkSemaphore sem;

static VkResult
waitfor(uint64_t v)
{
	VkSemaphoreWaitInfo wi = { VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO };

	wi.semaphoreCount = 1;
	wi.pSemaphores = &sem;
	wi.pValues = &v;
	return vkWaitSemaphores(dev, &wi, LIMIT);
}

static void *
signaller(void *arg)
{
	VkSemaphoreSignalInfo si = { VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO };

	(void)arg;
	usleep(200 * 1000);
	si.semaphore = sem;
	si.value = 4;
	vkSignalSemaphore(dev, &si);
	return NULL;
}

int
main(void)
{
	VkApplicationInfo app = { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	VkInstanceCreateInfo ici = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
	VkInstance inst;
	VkPhysicalDevice pd;
	VkPhysicalDeviceProperties props;
	VkPhysicalDeviceVulkan12Features f12 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
	VkPhysicalDeviceFeatures2 f2 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
	float prio = 1;
	VkDeviceQueueCreateInfo qci = { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
	VkDeviceCreateInfo dci = { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
	VkQueue q;
	VkSemaphoreTypeCreateInfo st = { VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO };
	VkSemaphoreCreateInfo sci = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
	VkTimelineSemaphoreSubmitInfo ts = { VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO };
	VkSubmitInfo si = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
	VkSemaphoreSignalInfo hs = { VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO };
	VkPipelineStageFlags stage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
	pthread_t t;
	uint32_t n = 1;
	uint64_t one = 1, v, five = 5, wait4 = 4;
	VkResult r;

	app.apiVersion = VK_API_VERSION_1_2;
	ici.pApplicationInfo = &app;
	if (vkCreateInstance(&ici, NULL, &inst) != VK_SUCCESS || vkEnumeratePhysicalDevices(inst, &n, &pd) < 0 || n < 1) {
		printf("FAIL no Vulkan device\nvktimeline: failed\n");
		return 1;
	}
	vkGetPhysicalDeviceProperties(pd, &props);
	f2.pNext = &f12;
	vkGetPhysicalDeviceFeatures2(pd, &f2);
	if (!f12.timelineSemaphore) {
		printf("FAIL %s: no timelineSemaphore\nvktimeline: failed\n", props.deviceName);
		return 1;
	}
	OK("%s: timelineSemaphore", props.deviceName);
	memset(&f12, 0, sizeof f12);
	f12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	f12.timelineSemaphore = VK_TRUE;
	qci.queueFamilyIndex = 0;
	qci.queueCount = 1;
	qci.pQueuePriorities = &prio;
	dci.pNext = &f12;
	dci.queueCreateInfoCount = 1;
	dci.pQueueCreateInfos = &qci;
	if (vkCreateDevice(pd, &dci, NULL, &dev) != VK_SUCCESS) {
		printf("FAIL vkCreateDevice\nvktimeline: failed\n");
		return 1;
	}
	vkGetDeviceQueue(dev, 0, 0, &q);
	st.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	st.initialValue = 0;
	sci.pNext = &st;
	if (vkCreateSemaphore(dev, &sci, NULL, &sem) != VK_SUCCESS) {
		printf("FAIL vkCreateSemaphore (timeline)\nvktimeline: failed\n");
		return 1;
	}

	/* the queue signals 1 */
	ts.signalSemaphoreValueCount = 1;
	ts.pSignalSemaphoreValues = &one;
	si.pNext = &ts;
	si.signalSemaphoreCount = 1;
	si.pSignalSemaphores = &sem;
	if ((r = vkQueueSubmit(q, 1, &si, VK_NULL_HANDLE)) != VK_SUCCESS)
		FAIL("vkQueueSubmit signalling 1: %d", r);
	else if ((r = waitfor(1)) != VK_SUCCESS)
		FAIL("vkWaitSemaphores for the queue's 1: %d (%s)", r, r == VK_TIMEOUT ? "2 s, no signal" : "error");
	else
		OK("the queue's signal 1 waited for");
	if (vkGetSemaphoreCounterValue(dev, sem, &v) != VK_SUCCESS || v != 1)
		FAIL("vkGetSemaphoreCounterValue: %llu, want 1", (unsigned long long)v);
	else
		OK("counter value 1");

	/* the processor signals 2 */
	hs.semaphore = sem;
	hs.value = 2;
	if ((r = vkSignalSemaphore(dev, &hs)) != VK_SUCCESS)
		FAIL("vkSignalSemaphore 2: %d", r);
	else if ((r = waitfor(2)) != VK_SUCCESS)
		FAIL("vkWaitSemaphores for the host's 2: %d", r);
	else
		OK("the host's signal 2 waited for");

	/* a wait for a signal still to come, from another thread */
	pthread_create(&t, NULL, signaller, NULL);
	if ((r = waitfor(4)) != VK_SUCCESS)
		FAIL("vkWaitSemaphores for 4, signalled 0.2 s later: %d", r);
	else
		OK("a signal from another thread, 0.2 s later, waited for");
	pthread_join(t, NULL);

	/* the queue waits for 4 (there) and signals 5 */
	memset(&ts, 0, sizeof ts);
	ts.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
	ts.waitSemaphoreValueCount = 1;
	ts.pWaitSemaphoreValues = &wait4;
	ts.signalSemaphoreValueCount = 1;
	ts.pSignalSemaphoreValues = &five;
	si.pNext = &ts;
	si.waitSemaphoreCount = 1;
	si.pWaitSemaphores = &sem;
	si.pWaitDstStageMask = &stage;
	si.signalSemaphoreCount = 1;
	si.pSignalSemaphores = &sem;
	if ((r = vkQueueSubmit(q, 1, &si, VK_NULL_HANDLE)) != VK_SUCCESS)
		FAIL("vkQueueSubmit 4 -> 5: %d", r);
	else if ((r = waitfor(5)) != VK_SUCCESS)
		FAIL("vkWaitSemaphores for the queue's 5 after its wait for 4: %d", r);
	else
		OK("the queue waited for 4 and signalled 5");
	if ((r = vkDeviceWaitIdle(dev)) != VK_SUCCESS)
		FAIL("vkDeviceWaitIdle: %d", r);
	else
		OK("vkDeviceWaitIdle");

	vkDestroySemaphore(dev, sem, NULL);
	vkDestroyDevice(dev, NULL);
	vkDestroyInstance(inst, NULL);
	printf(nfail ? "vktimeline: %d failed\n" : "vktimeline: all passed\n", nfail);
	return nfail != 0;
}
