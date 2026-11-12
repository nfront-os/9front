/* SPDX-License-Identifier: MIT */
/* LinuxKPI for 9front: i915_trace.h, made by scripts/linux/gentrace.py: no tracing */
#ifndef _KPI_TRACE_I915_TRACE_H
#define _KPI_TRACE_I915_TRACE_H

#include <linux/stringify.h>
#include <linux/types.h>
#include <drm/drm_drv.h>
#include "drivers/gpu/drm/i915/gt/intel_engine.h"
#include "drivers/gpu/drm/i915/i915_drv.h"

#define trace_amdgpu_bo_create(...)	do { } while (0)
#define trace_amdgpu_bo_create_enabled()	0
#define trace_amdgpu_bo_list_set(...)	do { } while (0)
#define trace_amdgpu_bo_list_set_enabled()	0
#define trace_amdgpu_bo_move(...)	do { } while (0)
#define trace_amdgpu_bo_move_enabled()	0
#define trace_amdgpu_cleaner_shader(...)	do { } while (0)
#define trace_amdgpu_cleaner_shader_enabled()	0
#define trace_amdgpu_cs(...)	do { } while (0)
#define trace_amdgpu_cs_enabled()	0
#define trace_amdgpu_cs_bo_status(...)	do { } while (0)
#define trace_amdgpu_cs_bo_status_enabled()	0
#define trace_amdgpu_cs_ioctl(...)	do { } while (0)
#define trace_amdgpu_cs_ioctl_enabled()	0
#define trace_amdgpu_dc_performance(...)	do { } while (0)
#define trace_amdgpu_dc_performance_enabled()	0
#define trace_amdgpu_dc_rreg(...)	do { } while (0)
#define trace_amdgpu_dc_rreg_enabled()	0
#define trace_amdgpu_dc_wreg(...)	do { } while (0)
#define trace_amdgpu_dc_wreg_enabled()	0
#define trace_amdgpu_device_rreg(...)	do { } while (0)
#define trace_amdgpu_device_rreg_enabled()	0
#define trace_amdgpu_device_wreg(...)	do { } while (0)
#define trace_amdgpu_device_wreg_enabled()	0
#define trace_amdgpu_dm_atomic_check_begin(...)	do { } while (0)
#define trace_amdgpu_dm_atomic_check_begin_enabled()	0
#define trace_amdgpu_dm_atomic_check_finish(...)	do { } while (0)
#define trace_amdgpu_dm_atomic_check_finish_enabled()	0
#define trace_amdgpu_dm_atomic_commit_tail_begin(...)	do { } while (0)
#define trace_amdgpu_dm_atomic_commit_tail_begin_enabled()	0
#define trace_amdgpu_dm_atomic_commit_tail_finish(...)	do { } while (0)
#define trace_amdgpu_dm_atomic_commit_tail_finish_enabled()	0
#define trace_amdgpu_dm_atomic_update_cursor(...)	do { } while (0)
#define trace_amdgpu_dm_atomic_update_cursor_enabled()	0
#define trace_amdgpu_dm_brightness(...)	do { } while (0)
#define trace_amdgpu_dm_brightness_enabled()	0
#define trace_amdgpu_dm_connector_atomic_check(...)	do { } while (0)
#define trace_amdgpu_dm_connector_atomic_check_enabled()	0
#define trace_amdgpu_dm_crtc_atomic_check(...)	do { } while (0)
#define trace_amdgpu_dm_crtc_atomic_check_enabled()	0
#define trace_amdgpu_dm_dc_clocks_state(...)	do { } while (0)
#define trace_amdgpu_dm_dc_clocks_state_enabled()	0
#define trace_amdgpu_dm_dc_pipe_state(...)	do { } while (0)
#define trace_amdgpu_dm_dc_pipe_state_enabled()	0
#define trace_amdgpu_dm_dce_clocks_state(...)	do { } while (0)
#define trace_amdgpu_dm_dce_clocks_state_enabled()	0
#define trace_amdgpu_dm_ism_commit(...)	do { } while (0)
#define trace_amdgpu_dm_ism_commit_enabled()	0
#define trace_amdgpu_dm_ism_event(...)	do { } while (0)
#define trace_amdgpu_dm_ism_event_enabled()	0
#define trace_amdgpu_dm_plane_atomic_check(...)	do { } while (0)
#define trace_amdgpu_dm_plane_atomic_check_enabled()	0
#define trace_amdgpu_dmub_trace_high_irq(...)	do { } while (0)
#define trace_amdgpu_dmub_trace_high_irq_enabled()	0
#define trace_amdgpu_ib_pipe_sync(...)	do { } while (0)
#define trace_amdgpu_ib_pipe_sync_enabled()	0
#define trace_amdgpu_isolation(...)	do { } while (0)
#define trace_amdgpu_isolation_enabled()	0
#define trace_amdgpu_iv(...)	do { } while (0)
#define trace_amdgpu_iv_enabled()	0
#define trace_amdgpu_pasid_allocated(...)	do { } while (0)
#define trace_amdgpu_pasid_allocated_enabled()	0
#define trace_amdgpu_pasid_freed(...)	do { } while (0)
#define trace_amdgpu_pasid_freed_enabled()	0
#define trace_amdgpu_refresh_rate_track(...)	do { } while (0)
#define trace_amdgpu_refresh_rate_track_enabled()	0
#define trace_amdgpu_sched_run_job(...)	do { } while (0)
#define trace_amdgpu_sched_run_job_enabled()	0
#define trace_amdgpu_userq_create_end(...)	do { } while (0)
#define trace_amdgpu_userq_create_end_enabled()	0
#define trace_amdgpu_userq_create_start(...)	do { } while (0)
#define trace_amdgpu_userq_create_start_enabled()	0
#define trace_amdgpu_userq_destroy_end(...)	do { } while (0)
#define trace_amdgpu_userq_destroy_end_enabled()	0
#define trace_amdgpu_userq_destroy_start(...)	do { } while (0)
#define trace_amdgpu_userq_destroy_start_enabled()	0
#define trace_amdgpu_userq_emit_fence(...)	do { } while (0)
#define trace_amdgpu_userq_emit_fence_enabled()	0
#define trace_amdgpu_userq_state_changed(...)	do { } while (0)
#define trace_amdgpu_userq_state_changed_enabled()	0
#define trace_amdgpu_userq_state_start(...)	do { } while (0)
#define trace_amdgpu_userq_state_start_enabled()	0
#define trace_amdgpu_userq_wait_deps(...)	do { } while (0)
#define trace_amdgpu_userq_wait_deps_enabled()	0
#define trace_amdgpu_vm_bo_cs(...)	do { } while (0)
#define trace_amdgpu_vm_bo_cs_enabled()	0
#define trace_amdgpu_vm_bo_map(...)	do { } while (0)
#define trace_amdgpu_vm_bo_map_enabled()	0
#define trace_amdgpu_vm_bo_mapping(...)	do { } while (0)
#define trace_amdgpu_vm_bo_mapping_enabled()	0
#define trace_amdgpu_vm_bo_unmap(...)	do { } while (0)
#define trace_amdgpu_vm_bo_unmap_enabled()	0
#define trace_amdgpu_vm_bo_update(...)	do { } while (0)
#define trace_amdgpu_vm_bo_update_enabled()	0
#define trace_amdgpu_vm_copy_ptes(...)	do { } while (0)
#define trace_amdgpu_vm_copy_ptes_enabled()	0
#define trace_amdgpu_vm_flush(...)	do { } while (0)
#define trace_amdgpu_vm_flush_enabled()	0
#define trace_amdgpu_vm_grab_id(...)	do { } while (0)
#define trace_amdgpu_vm_grab_id_enabled()	0
#define trace_amdgpu_vm_set_ptes(...)	do { } while (0)
#define trace_amdgpu_vm_set_ptes_enabled()	0
#define trace_amdgpu_vm_update_ptes(...)	do { } while (0)
#define trace_amdgpu_vm_update_ptes_enabled()	0
#define trace_dcn_fpu(...)	do { } while (0)
#define trace_dcn_fpu_enabled()	0
#define trace_dcn_optc_lock_unlock_state(...)	do { } while (0)
#define trace_dcn_optc_lock_unlock_state_enabled()	0
#define trace_dma_fence_signaled(...)	do { } while (0)
#define trace_dma_fence_signaled_enabled()	0
#define trace_drm_sched_job_add_dep(...)	do { } while (0)
#define trace_drm_sched_job_add_dep_enabled()	0
#define trace_drm_sched_job_done(...)	do { } while (0)
#define trace_drm_sched_job_done_enabled()	0
#define trace_drm_sched_job_queue(...)	do { } while (0)
#define trace_drm_sched_job_queue_enabled()	0
#define trace_drm_sched_job_run(...)	do { } while (0)
#define trace_drm_sched_job_run_enabled()	0
#define trace_drm_sched_job_unschedulable(...)	do { } while (0)
#define trace_drm_sched_job_unschedulable_enabled()	0
#define trace_drm_vblank_event(...)	do { } while (0)
#define trace_drm_vblank_event_enabled()	0
#define trace_drm_vblank_event_delivered(...)	do { } while (0)
#define trace_drm_vblank_event_delivered_enabled()	0
#define trace_drm_vblank_event_queued(...)	do { } while (0)
#define trace_drm_vblank_event_queued_enabled()	0
#define trace_g4x_wm(...)	do { } while (0)
#define trace_g4x_wm_enabled()	0
#define trace_i915_context_create(...)	do { } while (0)
#define trace_i915_context_create_enabled()	0
#define trace_i915_context_free(...)	do { } while (0)
#define trace_i915_context_free_enabled()	0
#define trace_i915_gem_evict(...)	do { } while (0)
#define trace_i915_gem_evict_enabled()	0
#define trace_i915_gem_evict_node(...)	do { } while (0)
#define trace_i915_gem_evict_node_enabled()	0
#define trace_i915_gem_evict_vm(...)	do { } while (0)
#define trace_i915_gem_evict_vm_enabled()	0
#define trace_i915_gem_object_clflush(...)	do { } while (0)
#define trace_i915_gem_object_clflush_enabled()	0
#define trace_i915_gem_object_create(...)	do { } while (0)
#define trace_i915_gem_object_create_enabled()	0
#define trace_i915_gem_object_destroy(...)	do { } while (0)
#define trace_i915_gem_object_destroy_enabled()	0
#define trace_i915_gem_object_fault(...)	do { } while (0)
#define trace_i915_gem_object_fault_enabled()	0
#define trace_i915_gem_object_pread(...)	do { } while (0)
#define trace_i915_gem_object_pread_enabled()	0
#define trace_i915_gem_object_pwrite(...)	do { } while (0)
#define trace_i915_gem_object_pwrite_enabled()	0
#define trace_i915_gem_shrink(...)	do { } while (0)
#define trace_i915_gem_shrink_enabled()	0
#define trace_i915_ppgtt_create(...)	do { } while (0)
#define trace_i915_ppgtt_create_enabled()	0
#define trace_i915_ppgtt_release(...)	do { } while (0)
#define trace_i915_ppgtt_release_enabled()	0
#define trace_i915_reg_rw(...)	do { } while (0)
#define trace_i915_reg_rw_enabled()	0
#define trace_i915_request_add(...)	do { } while (0)
#define trace_i915_request_add_enabled()	0
#define trace_i915_request_execute(...)	do { } while (0)
#define trace_i915_request_execute_enabled()	0
#define trace_i915_request_guc_submit(...)	do { } while (0)
#define trace_i915_request_guc_submit_enabled()	0
#define trace_i915_request_in(...)	do { } while (0)
#define trace_i915_request_in_enabled()	0
#define trace_i915_request_out(...)	do { } while (0)
#define trace_i915_request_out_enabled()	0
#define trace_i915_request_queue(...)	do { } while (0)
#define trace_i915_request_queue_enabled()	0
#define trace_i915_request_retire(...)	do { } while (0)
#define trace_i915_request_retire_enabled()	0
#define trace_i915_request_submit(...)	do { } while (0)
#define trace_i915_request_submit_enabled()	0
#define trace_i915_request_wait_begin(...)	do { } while (0)
#define trace_i915_request_wait_begin_enabled()	0
#define trace_i915_request_wait_end(...)	do { } while (0)
#define trace_i915_request_wait_end_enabled()	0
#define trace_i915_vma_bind(...)	do { } while (0)
#define trace_i915_vma_bind_enabled()	0
#define trace_i915_vma_unbind(...)	do { } while (0)
#define trace_i915_vma_unbind_enabled()	0
#define trace_intel_context_ban(...)	do { } while (0)
#define trace_intel_context_ban_enabled()	0
#define trace_intel_context_create(...)	do { } while (0)
#define trace_intel_context_create_enabled()	0
#define trace_intel_context_deregister(...)	do { } while (0)
#define trace_intel_context_deregister_enabled()	0
#define trace_intel_context_deregister_done(...)	do { } while (0)
#define trace_intel_context_deregister_done_enabled()	0
#define trace_intel_context_do_pin(...)	do { } while (0)
#define trace_intel_context_do_pin_enabled()	0
#define trace_intel_context_do_unpin(...)	do { } while (0)
#define trace_intel_context_do_unpin_enabled()	0
#define trace_intel_context_fence_release(...)	do { } while (0)
#define trace_intel_context_fence_release_enabled()	0
#define trace_intel_context_free(...)	do { } while (0)
#define trace_intel_context_free_enabled()	0
#define trace_intel_context_register(...)	do { } while (0)
#define trace_intel_context_register_enabled()	0
#define trace_intel_context_reset(...)	do { } while (0)
#define trace_intel_context_reset_enabled()	0
#define trace_intel_context_sched_disable(...)	do { } while (0)
#define trace_intel_context_sched_disable_enabled()	0
#define trace_intel_context_sched_done(...)	do { } while (0)
#define trace_intel_context_sched_done_enabled()	0
#define trace_intel_context_sched_enable(...)	do { } while (0)
#define trace_intel_context_sched_enable_enabled()	0
#define trace_intel_context_set_prio(...)	do { } while (0)
#define trace_intel_context_set_prio_enabled()	0
#define trace_intel_context_steal_guc_id(...)	do { } while (0)
#define trace_intel_context_steal_guc_id_enabled()	0
#define trace_intel_cpu_fifo_underrun(...)	do { } while (0)
#define trace_intel_cpu_fifo_underrun_enabled()	0
#define trace_intel_crtc_flip_done(...)	do { } while (0)
#define trace_intel_crtc_flip_done_enabled()	0
#define trace_intel_crtc_vblank_work_end(...)	do { } while (0)
#define trace_intel_crtc_vblank_work_end_enabled()	0
#define trace_intel_crtc_vblank_work_start(...)	do { } while (0)
#define trace_intel_crtc_vblank_work_start_enabled()	0
#define trace_intel_fbc_activate(...)	do { } while (0)
#define trace_intel_fbc_activate_enabled()	0
#define trace_intel_fbc_deactivate(...)	do { } while (0)
#define trace_intel_fbc_deactivate_enabled()	0
#define trace_intel_fbc_nuke(...)	do { } while (0)
#define trace_intel_fbc_nuke_enabled()	0
#define trace_intel_frontbuffer_flush(...)	do { } while (0)
#define trace_intel_frontbuffer_flush_enabled()	0
#define trace_intel_frontbuffer_invalidate(...)	do { } while (0)
#define trace_intel_frontbuffer_invalidate_enabled()	0
#define trace_intel_memory_cxsr(...)	do { } while (0)
#define trace_intel_memory_cxsr_enabled()	0
#define trace_intel_pch_fifo_underrun(...)	do { } while (0)
#define trace_intel_pch_fifo_underrun_enabled()	0
#define trace_intel_pipe_crc(...)	do { } while (0)
#define trace_intel_pipe_crc_enabled()	0
#define trace_intel_pipe_disable(...)	do { } while (0)
#define trace_intel_pipe_disable_enabled()	0
#define trace_intel_pipe_enable(...)	do { } while (0)
#define trace_intel_pipe_enable_enabled()	0
#define trace_intel_pipe_scaler_update_arm(...)	do { } while (0)
#define trace_intel_pipe_scaler_update_arm_enabled()	0
#define trace_intel_pipe_update_end(...)	do { } while (0)
#define trace_intel_pipe_update_end_enabled()	0
#define trace_intel_pipe_update_start(...)	do { } while (0)
#define trace_intel_pipe_update_start_enabled()	0
#define trace_intel_pipe_update_vblank_evaded(...)	do { } while (0)
#define trace_intel_pipe_update_vblank_evaded_enabled()	0
#define trace_intel_plane_async_flip(...)	do { } while (0)
#define trace_intel_plane_async_flip_enabled()	0
#define trace_intel_plane_disable_arm(...)	do { } while (0)
#define trace_intel_plane_disable_arm_enabled()	0
#define trace_intel_plane_scaler_update_arm(...)	do { } while (0)
#define trace_intel_plane_scaler_update_arm_enabled()	0
#define trace_intel_plane_update_arm(...)	do { } while (0)
#define trace_intel_plane_update_arm_enabled()	0
#define trace_intel_plane_update_noarm(...)	do { } while (0)
#define trace_intel_plane_update_noarm_enabled()	0
#define trace_intel_scaler_disable_arm(...)	do { } while (0)
#define trace_intel_scaler_disable_arm_enabled()	0
#define trace_printk(...)	do { } while (0)
#define trace_printk_enabled()	0
#define trace_vlv_fifo_size(...)	do { } while (0)
#define trace_vlv_fifo_size_enabled()	0
#define trace_vlv_wm(...)	do { } while (0)
#define trace_vlv_wm_enabled()	0

#endif
