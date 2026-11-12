// SPDX-License-Identifier: MIT
/*
 * LinuxKPI for 9front: amdgpu display's dc/dce/dmub_abm_cacp.c (ours:
 * Linux 7.3's carries no licence, so we have none of its code).  CACP is
 * not supported here: every request fails, so a panel that offers it
 * goes without (its backlight still works through ABM's other paths
 * where dmub_abm takes them).
 */
#include "dmub_abm_cacp.h"

void
dmub_cacp_init(struct abm *abm, const char *src, unsigned int bytes, unsigned int panel_inst)
{
}

bool
dmub_cacp_set_level(struct abm *abm, unsigned int cacp_level, unsigned char panel_mask)
{
	return false;
}

bool
dmub_cacp_set_pipe(struct abm *abm, unsigned int otg_inst, unsigned int pipe_option,
	unsigned int panel_inst, unsigned int pwrseq_inst)
{
	return false;
}

bool
dmub_cacp_set_event(struct abm *abm, unsigned int full_screen, unsigned int trans_info,
	unsigned int hdr_mode, unsigned int scaling_enable, unsigned int panel_inst)
{
	return false;
}

bool
dmub_cacp_set_pause(struct abm *abm, bool pause, unsigned int panel_inst, unsigned int otg_inst)
{
	return false;
}

bool
dmub_cacp_set_backlight_level(struct abm *abm, unsigned int backlight_pwm_u16_16,
	unsigned int frame_ramp, unsigned int panel_inst)
{
	return false;
}

void
dmub_cacp_enable_fractional_pwm(struct abm *abm, uint8_t panel_mask)
{
}
