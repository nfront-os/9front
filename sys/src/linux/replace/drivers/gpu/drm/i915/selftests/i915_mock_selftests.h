/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: i915's mock selftests, our own list (Linux's is
 * GPL): selftest(name, function), run in this order by
 * i915_mock_selftests() (selftests/i915_selftest.c); a test that runs
 * without hardware, on mock_gem_device().  The sanity check first, as
 * Linux asks; the others by name.
 */
#ifndef selftest
#define selftest(x, y)
#endif
selftest(sanitycheck, i915_mock_sanitycheck)
selftest(dmabuf, i915_gem_dmabuf_mock_selftests)
selftest(engine, intel_engine_cs_mock_selftests)
selftest(evict, i915_gem_evict_mock_selftests)
selftest(fence, i915_sw_fence_mock_selftests)
selftest(gtt, i915_gem_gtt_mock_selftests)
selftest(hugepages, i915_gem_huge_page_mock_selftests)
selftest(memory_region, intel_memory_region_mock_selftests)
selftest(objects, i915_gem_object_mock_selftests)
selftest(phys, i915_gem_phys_mock_selftests)
selftest(requests, i915_request_mock_selftests)
selftest(ring, intel_ring_mock_selftests)
selftest(scatterlist, scatterlist_mock_selftests)
selftest(shmem, shmem_utils_mock_selftests)
selftest(syncmap, i915_syncmap_mock_selftests)
selftest(timelines, intel_timeline_mock_selftests)
selftest(uncore, intel_uncore_mock_selftests)
selftest(vma, i915_vma_mock_selftests)
