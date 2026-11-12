/* SPDX-License-Identifier: MIT */
/*
 * LinuxKPI for 9front: amdgpu display's dc/dce/dmub_abm_cacp.h (ours:
 * Linux 7.3's carries no licence).  CACP, the ABM path dmub_abm takes for
 * eDP panels that report cacp_supported; here not supported
 * (dmub_abm_cacp.c).
 */
#ifndef _KPI_DMUB_ABM_CACP_H
#define _KPI_DMUB_ABM_CACP_H

#include "dmub_cmd.h"

struct abm;

/*
 * what dmub_abm.c calls when the panel has CACP, by name: the arguments
 * are panel and pipe numbers, levels, flags, as dmub_abm.c passes them
 */
void	dmub_cacp_enable_fractional_pwm(struct abm *, uint8_t);
void	dmub_cacp_init(struct abm *, const char *, unsigned int, unsigned int);
bool	dmub_cacp_set_backlight_level(struct abm *, unsigned int, unsigned int, unsigned int);
bool	dmub_cacp_set_event(struct abm *, unsigned int, unsigned int, unsigned int,
	    unsigned int, unsigned int);
bool	dmub_cacp_set_level(struct abm *, unsigned int, unsigned char);
bool	dmub_cacp_set_pause(struct abm *, bool, unsigned int, unsigned int);
bool	dmub_cacp_set_pipe(struct abm *, unsigned int, unsigned int, unsigned int, unsigned int);

#endif
