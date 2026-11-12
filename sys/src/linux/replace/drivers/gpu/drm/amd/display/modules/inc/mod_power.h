/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: amdgpu display's modules/inc/mod_power.h, written
 * here (Linux 7.3's carries no licence, "All rights reserved" only).
 *
 * The power module drives what saves power on built-in (eDP) panels:
 * the backlight (in PWM steps, percent or nits), ABM (adaptive backlight,
 * "varibright"), PSR (panel self refresh) and panel replay.  amdgpu_dm
 * calls it, modules/power/*.c (MIT) implement it.  Only what those use
 * is here: the init parameters, the PSR context fields the module reads,
 * the events, the calls.
 */
#ifndef _KPI_MODULES_INC_MOD_POWER_H
#define _KPI_MODULES_INC_MOD_POWER_H

#include "dm_services.h"

struct dc;
struct dc_link;
struct dc_stream_state;
struct dc_stream_update;
struct psr_caps;

/* the handle mod_power_create gives (the module's state lies around it) */
struct mod_power {
	int	unused;
};

/* the panel's backlight and the module's defaults, for mod_power_create */
struct mod_power_init_params {
	bool	disable_fractional_pwm;
	bool	use_nits_based_brightness;
	unsigned int	panel_min_millinits;
	unsigned int	panel_max_millinits;
	unsigned int	min_backlight_pwm;
	unsigned int	max_backlight_pwm;
	unsigned int	min_abm_backlight;
	unsigned int	num_backlight_levels;
	bool	backlight_ramping_override;
	unsigned int	backlight_ramping_reduction;
	unsigned int	backlight_ramping_start;
	bool	def_varibright_enable;
	unsigned int	def_varibright_level;
	unsigned int	varibright_level;
	unsigned int	abm_config_setting;
	bool	allow_psr_smu_optimizations;
	bool	allow_psr_multi_disp_optimizations;
	bool	use_custom_backlight_caps;
	unsigned int	custom_backlight_caps_config_no;
	bool	use_linear_backlight_curve;
};

/* what the module keeps per stream for PSR (only within the module) */
struct mod_power_psr_context {
	bool	psr_exit_link_training_required;
	bool	psr_frame_capture_indication_req;
	bool	allow_smu_optimizations;
	bool	allow_multi_disp_optimizations;
	bool	su_granularity_required;
	bool	os_request_force_ffu;
	uint8_t	su_y_granularity;
	uint8_t	rate_control_caps;
	unsigned int	line_time_in_us;
};

/* reasons to hold PSR off, one bit each (set and cleared by mod_power_set_psr_event) */
enum psr_event {
	psr_event_vsync				= 1 << 0,
	psr_event_full_screen			= 1 << 1,
	psr_event_defer_enable			= 1 << 2,
	psr_event_hw_programming		= 1 << 3,
	psr_event_test_harness_enable_psr	= 1 << 4,
	psr_event_test_harness_disable_psr	= 1 << 5,
	psr_event_mpo_video_selective_update	= 1 << 6,
	psr_event_edp_panel_off_disable_psr	= 1 << 7,
	psr_event_dynamic_display_switch	= 1 << 8,
	psr_event_big_screen_video		= 1 << 9,
	psr_event_dds_defer_stream_enable	= 1 << 11,
	psr_event_dynamic_link_rate_control	= 1 << 12,
	psr_event_vrr_transition		= 1 << 13,
	psr_event_pause				= 1 << 14,
	psr_event_immediate_flip		= 1 << 15,
	psr_event_os_request_disable		= 1 << 16,
	psr_event_os_request_force_ffu		= 1 << 17,
	psr_event_os_override_hold		= 1 << 18,
	psr_event_crc_window_active		= 1 << 19,
};

/* the same for panel replay (mod_power_set_replay_event) */
enum replay_event {
	replay_event_vsync				= 1 << 0,
	replay_event_full_screen			= 1 << 1,
	replay_event_mpo_video_selective_update		= 1 << 2,
	replay_event_big_screen_video			= 1 << 3,
	replay_event_hw_programming			= 1 << 4,
	replay_event_edp_panel_off_disable_psr		= 1 << 5,
	replay_event_general_ui				= 1 << 6,
	replay_event_vrr				= 1 << 7,
	replay_event_prepare_vtotal			= 1 << 8,
	replay_event_test_harness_enable_replay		= 1 << 9,
	replay_event_test_harness_disable_replay	= 1 << 10,
	replay_event_test_harness_ultra_sleep		= 1 << 11,
	replay_event_immediate_flip			= 1 << 12,
	replay_event_vrr_transition			= 1 << 13,
	replay_event_pause				= 1 << 14,
	replay_event_disable_replay_while_DPMS		= 1 << 15,
	replay_event_test_harness_mode			= 1 << 16,
	replay_event_cursor_updating			= 1 << 17,
	replay_event_sleep_resume			= 1 << 18,
	replay_event_disable_in_AC			= 1 << 19,
	replay_event_disable_replay_while_detect_display = 1 << 20,
	replay_event_disable_replay_while_switching_mux	= 1 << 22,
	replay_event_infopacket				= 1 << 23,
	replay_event_os_request_disable			= 1 << 24,
	replay_event_os_request_force_ffu		= 1 << 25,
	replay_event_os_override_hold			= 1 << 26,
	replay_event_crc_window_active			= 1 << 27,
};

/* when panel replay may run (the replay options of a stream) */
enum replay_enable_option {
	pr_enable_option_static_screen			= 1 << 0,
	pr_enable_option_mpo_video			= 1 << 1,
	pr_enable_option_full_screen_video		= 1 << 2,
	pr_enable_option_general_ui			= 1 << 3,
	pr_enable_option_full_screen			= 1 << 4,
	pr_enable_option_static_screen_coasting		= 1 << 16,
	pr_enable_option_mpo_video_coasting		= 1 << 17,
	pr_enable_option_full_screen_video_coasting	= 1 << 18,
};

/* the module, its streams */
struct mod_power *mod_power_create(struct dc *dc, struct mod_power_init_params *p, unsigned int edp_num);
void	mod_power_destroy(struct mod_power *m);
bool	mod_power_hw_init(struct mod_power *m);
bool	mod_power_add_stream(struct mod_power *m, struct dc_stream_state *stream, struct psr_caps *caps);
bool	mod_power_remove_stream(struct mod_power *m, const struct dc_stream_state *stream);
bool	mod_power_replace_stream(struct mod_power *m, const struct dc_stream_state *cur,
	struct dc_stream_state *new_stream, struct psr_caps *new_caps);
bool	mod_power_notify_mode_change(struct mod_power *m, const struct dc_stream_state *stream, bool is_hdr);

/* the backlight: set and read, in PWM (16.16), millipercent or millinits; per panel inst */
bool	mod_power_set_backlight_nits(struct mod_power *m, struct dc_stream_state *streams,
	unsigned int millinits, unsigned int transition_ms, bool skip_aux, bool is_hdr);
bool	mod_power_set_backlight_percent(struct mod_power *m, struct dc_stream_state *stream,
	unsigned int millipercent, unsigned int transition_ms, bool is_hdr);
void	mod_power_update_backlight(struct mod_power *m, struct dc_stream_state *stream, unsigned int millipercent);
void	mod_power_update_backlight_nits(struct mod_power *m, struct dc_stream_state *stream, unsigned int millinits);
bool	mod_power_get_backlight_pwm(struct mod_power *m, unsigned int *pwm, unsigned int inst);
bool	mod_power_get_backlight_nits(struct mod_power *m, unsigned int *millinits, unsigned int inst);
bool	mod_power_get_backlight_percent(struct mod_power *m, unsigned int *millipercent, unsigned int inst);
bool	mod_power_get_hw_target_backlight_pwm(struct mod_power *m, const struct dc_link *link, unsigned int *u16_16);
bool	mod_power_get_hw_target_backlight_pwm_nits(struct mod_power *m, const struct dc_link *link,
	unsigned int *millinits, unsigned int inst);
bool	mod_power_get_hw_target_backlight_pwm_percent(struct mod_power *m, const struct dc_link *link,
	unsigned int *millipercent, unsigned int inst);
bool	mod_power_get_hw_backlight_pwm(struct mod_power *m, const struct dc_link *link, unsigned int *pwm);
bool	mod_power_get_hw_backlight_pwm_nits(struct mod_power *m, const struct dc_link *link,
	unsigned int *millinits, unsigned int inst);
bool	mod_power_get_hw_backlight_pwm_percent(struct mod_power *m, const struct dc_link *link,
	unsigned int *millipercent, unsigned int inst);
bool	mod_power_get_hw_backlight_aux_nits(struct mod_power *m, struct dc_stream_state **streams, int nstreams,
	unsigned int *avg_millinits, unsigned int *peak_millinits);
bool	mod_power_get_panel_backlight_boundaries(struct mod_power *m, unsigned int *min, unsigned int *max,
	unsigned int *ac_percent, unsigned int *dc_percent, unsigned int inst);
bool	mod_power_set_smooth_brightness(struct mod_power *m, bool enable, unsigned int inst);
bool	mod_power_backlight_percent_to_nits(struct mod_power *m, struct dc_stream_state *stream,
	unsigned int millipercent, unsigned int *millinits);
bool	mod_power_backlight_nits_to_percent(struct mod_power *m, struct dc_stream_state *stream,
	unsigned int millinits, unsigned int *millipercent);

/* ABM (varibright): its level and whether it is on */
bool	mod_power_get_varibright_level(struct mod_power *m, unsigned int *level);
bool	mod_power_get_varibright_hw_level(struct mod_power *m, unsigned int *level);
bool	mod_power_get_varibright_default_level(struct mod_power *m, unsigned int *level);
bool	mod_power_get_varibright_enable(struct mod_power *m, bool *enable);
bool	mod_power_varibright_activate(struct mod_power *m, bool activate, struct dc_stream_update *u);
bool	mod_power_varibright_feature_enable(struct mod_power *m, bool enable, struct dc_stream_update *u);
bool	mod_power_varibright_set_level(struct mod_power *m, unsigned int level, struct dc_stream_update *u);
bool	mod_power_varibright_set_hw_level(struct mod_power *m, unsigned int level, struct dc_stream_update *u);
bool	mod_power_is_abm_active(struct mod_power *m, const struct dc_link *link, unsigned int inst);
bool	mod_power_is_abm_supported(struct mod_power *m, unsigned int inst);
bool	mod_power_abm_set_event(struct mod_power *m, unsigned int full_screen, unsigned int trans_info,
	unsigned int hdr_mode, unsigned int scaling_enable, unsigned int scaling_strength_map, unsigned int inst);
bool	mod_power_abm_set_strength(struct mod_power *m, unsigned int strength, unsigned int inst);

/* PSR */
bool	mod_power_set_psr_event(struct mod_power *m, struct dc_stream_state *stream, bool set,
	enum psr_event event, bool wait);
bool	mod_power_get_psr_event(struct mod_power *m, struct dc_stream_state *stream, unsigned int *events);
bool	mod_power_get_psr_state(struct mod_power *m, const struct dc_stream_state *stream, enum dc_psr_state *state);
bool	mod_power_get_psr_enabled_status(struct mod_power *m, const struct dc_stream_state *stream, bool *enabled);
bool	mod_power_psr_get_active_psr_events(struct mod_power *m, const struct dc_stream_state *stream,
	unsigned int *events);
bool	mod_power_psr_set_sink_vtotal_in_psr_active(struct mod_power *m, const struct dc_stream_state *stream,
	uint16_t vtotal_idle, uint16_t vtotal_su);
void	mod_power_psr_residency(struct mod_power *m, const struct dc_stream_state *stream,
	unsigned int *residency, const uint8_t mode);

/* panel replay */
bool	mod_power_set_replay_event(struct mod_power *m, struct dc_stream_state *stream, bool set,
	enum replay_event event, bool wait_for_disable);
bool	mod_power_get_replay_event(struct mod_power *m, struct dc_stream_state *stream, unsigned int *events);
bool	mod_power_get_replay_active_status(const struct dc_stream_state *stream, bool *active);
bool	mod_power_replay_set_coasting_vtotal(struct mod_power *m, const struct dc_stream_state *stream,
	uint32_t coasting_vtotal, uint16_t frame_skip_number);
bool	mod_power_replay_set_power_opt_and_coasting_vtotal(struct mod_power *m, const struct dc_stream_state *stream,
	unsigned int events, uint32_t coasting_vtotal, bool ultra_sleep, uint16_t frame_skip_number);
void	mod_power_replay_residency(const struct dc_stream_state *stream, unsigned int *residency,
	const bool is_start, const bool is_alpm);
void	mod_power_replay_set_timing_sync_supported(struct mod_power *m, const struct dc_stream_state *stream);
void	mod_power_replay_disabled_adaptive_sync_sdp(struct mod_power *m, const struct dc_stream_state *stream,
	bool force_disabled);
void	mod_power_replay_disabled_desync_error_detection(struct mod_power *m, const struct dc_stream_state *stream,
	bool force_disabled);
void	mod_power_set_low_rr_activate(struct mod_power *m, const struct dc_stream_state *stream, bool supported);
void	mod_power_set_video_conferencing_activate(struct mod_power *m, const struct dc_stream_state *stream,
	bool activate);
void	mod_power_set_live_capture_with_cvt_activate(struct mod_power *m, const struct dc_stream_state *stream,
	bool activate);
void	mod_power_set_replay_continuously_resync(struct mod_power *m, const struct dc_stream_state *stream,
	bool enable);
void	mod_power_set_coasting_vtotal_without_frame_update(struct mod_power *m, const struct dc_stream_state *stream,
	uint32_t coasting_vtotal);

#endif
